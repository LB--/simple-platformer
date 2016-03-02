#ifndef simplat_ui_Title_HeaderPlusPlus
#define simplat_ui_Title_HeaderPlusPlus

#include <Magnum/Buffer.h>
#include <Magnum/Mesh.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/GlyphCache.h>
#include <Magnum/Text/Renderer.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/Shaders/DistanceFieldVector.h>

#include <string>
#include <tuple>

namespace simplat { namespace ui
{
	using Object2D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation2D>;
	using Magnum::SceneGraph::Drawable2D;
	struct Title final
	: Object2D
	, Drawable2D
	{
		Title(Magnum::SceneGraph::DrawableGroup2D &group, Magnum::Text::AbstractFont &font, Magnum::Text::GlyphCache &glyphs, Object2D *parent)
		: Object2D{parent}
		, Drawable2D{*this, &group}
		, font(font)
		, glyphs(glyphs)
		{
			std::tie(mesh, std::ignore) = Magnum::Text::Renderer2D::render
			(
				font, glyphs,
				0.1295f,
				"Simple Platformer",
				vertices, indices, Magnum::BufferUsage::StaticDraw,
				Magnum::Text::Alignment::TopCenter
			);
			shader
				.setVectorTexture(glyphs.texture())
				.setColor(Magnum::Color4{1.0f, 0.75f})
				.setOutlineColor(Magnum::Color4{0.0f, 0.7f, 0.0f, 0.75f})
				.setOutlineRange(0.45f, 0.35f)
				.setSmoothness(0.025f);
		}

	private:
		Magnum::Text::AbstractFont &font;
		Magnum::Text::GlyphCache &glyphs;
		Magnum::Mesh mesh {Magnum::NoCreate};
		Magnum::Buffer vertices, indices;
		Magnum::Shaders::DistanceFieldVector2D shader;
		Magnum::Matrix3 transform
			= Magnum::Matrix3::translation({0.0f, 0.5f})
			* Magnum::Matrix3::rotation(Magnum::Deg(-1.0f));

		virtual void draw(Magnum::Matrix3 const &transmat, Magnum::SceneGraph::Camera2D &cam) override
		{
			shader.setTransformationProjectionMatrix(cam.projectionMatrix() * transform);
			mesh.draw(shader);
		}
	};
}}

#endif
