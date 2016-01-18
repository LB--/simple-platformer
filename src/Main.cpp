#include "config.h"

#include "Resources.hpp"

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/Buffer.h>
#include <Magnum/Context.h>
#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Complex.h>
#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Renderer.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Text/Renderer.h>
#include <Magnum/Timeline.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Version.h>

#include <cstdlib>
#include <memory>
#include <tuple>

using Magnum::Math::operator""_degf;
using Magnum::Matrix3;
using Magnum::Matrix4;
using Magnum::Renderer;
using BlendFunction = Magnum::Renderer::BlendFunction;
using BlendEquation = Magnum::Renderer::BlendEquation;
namespace SceneGraph = Magnum::SceneGraph;
namespace Text = Magnum::Text;
using Magnum::Vector2;
using Magnum::Vector2i;
using Magnum::Vector3;

struct SimplePlatformer final
: Magnum::Platform::Application
{
	explicit SimplePlatformer(Arguments const &arguments)
	: Magnum::Platform::Application
		{
			arguments,
			Configuration()
				.setTitle("Simple Platformer")
				.setSize({1280, 720})
		}
	, font_plugins{MAGNUM_PLUGINS_FONT_DIR}
	, glyph_cache{Vector2i{2048}, Vector2i{512}, 22}
	, text_mesh{Magnum::NoCreate}
	{
		Magnum::Debug()
			<< "This application is running on"
			<< Magnum::Context::current().version()
			<< "using"
			<< Magnum::Context::current().rendererString();

		camera_object.translate(Vector3::zAxis(5.0f));
		camera
			.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
			.setProjectionMatrix(Matrix4::perspectiveProjection
			(
				35.0_degf,
				4.0f/3.0f,
				0.001f,
				100.0f
			))
			.setViewport(Magnum::defaultFramebuffer.viewport().size());

		font = font_plugins.loadAndInstantiate("FreeTypeFont");
		if(!font)
		{
			std::exit(EXIT_FAILURE);
		}

		Corrade::Utility::Resource res {"data"};
		if(!font->openSingleData(res.getRaw("data/OpenSans/OpenSans-Regular.ttf"), 110.0f))
		{
			Magnum::Error() << "Cannot open font file";
			std::exit(EXIT_FAILURE);
		}

		font->fillGlyphCache(glyph_cache, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-+,.!'\"\\/[]{}<>()|");
		std::tie(text_mesh, std::ignore) = Text::Renderer2D::render
		(
			*font, glyph_cache,
			0.1295f,
			"Hello, world!",
			vertexBuffer, indexBuffer, Magnum::BufferUsage::StaticDraw,
			Text::Alignment::MiddleCenter
		);

		text_renderer = std::make_unique<Text::Renderer2D>
		(
			*font,
			glyph_cache,
			0.035f,
			Text::Alignment::TopRight
		);
		text_renderer->reserve
		(
			40,
			Magnum::BufferUsage::DynamicDraw,
			Magnum::BufferUsage::StaticDraw
		);

		Renderer::enable(Renderer::Feature::DepthTest);
		Renderer::enable(Renderer::Feature::FaceCulling);

		Renderer::enable(Renderer::Feature::Blending);
		Renderer::setBlendFunction(BlendFunction::One, BlendFunction::OneMinusSourceAlpha);
		Renderer::setBlendEquation(BlendEquation::Add, BlendEquation::Add);

		text_transform = Matrix3::rotation(Magnum::Deg(-10.0f));
		text_project = Matrix3::scaling(Vector2::yScale(Vector2{Magnum::defaultFramebuffer.viewport().size()}.aspectRatio()));
		text_renderer->render("Simple Platformer");

		Magnum::Trade::MeshData3D cube = Magnum::Primitives::Cube::solid();

		vertexBuffer.setData(Magnum::MeshTools::interleave(cube.positions(0), cube.normals(0)), Magnum::BufferUsage::StaticDraw);

		Magnum::Containers::Array<char> indexData;
		Magnum::Mesh::IndexType indexType;
		Magnum::UnsignedInt indexStart, indexEnd;
		std::tie(indexData, indexType, indexStart, indexEnd) = Magnum::MeshTools::compressIndices(cube.indices());
		indexBuffer.setData(indexData, Magnum::BufferUsage::StaticDraw);

		mesh
			.setPrimitive(cube.primitive())
			.setCount(cube.indices().size())
			.addVertexBuffer(vertexBuffer, 0, Magnum::Shaders::Phong::Position{}, Magnum::Shaders::Phong::Normal{})
			.setIndexBuffer(indexBuffer, 0, indexType, indexStart, indexEnd);

		transformation
			= Matrix4::rotationX(Magnum::Deg(30.0f))
			* Matrix4::rotationY(Magnum::Deg(40.0f));
		color = Magnum::Color3::fromHSV(Magnum::Deg(35.0f), 1.0f, 1.0f);

		projection
			= Matrix4::perspectiveProjection
			(
				Magnum::Deg(35.0f),
				Vector2(Magnum::defaultFramebuffer.viewport().size()).aspectRatio(),
				0.01f,
				100.0f
			)
			* Matrix4::translation(Vector3::zAxis(-10.0f));

		setSwapInterval(1); //enable VSync
		timeline.start();
	}

private:
	virtual void drawEvent() override
	{
		Magnum::defaultFramebuffer.clear(Magnum::FramebufferClear::Color|Magnum::FramebufferClear::Depth);

		color = Magnum::Color3::fromHSV(color.hue() + Magnum::Deg(90.0*timeline.previousFrameDuration()), 1.0f, 1.0f);

		shader
			.setLightPosition({7.0f, 5.0f, 2.5f})
			.setLightColor(Magnum::Color3(1.0f))
			.setDiffuseColor(color)
			.setAmbientColor(Magnum::Color3::fromHSV(color.hue(), 1.0f, 0.3f))
			.setTransformationMatrix(transformation)
			.setNormalMatrix(transformation.rotationScaling())
			.setProjectionMatrix(projection);
		mesh.draw(shader);

		text_shader.setVectorTexture(glyph_cache.texture());
		text_shader
			.setTransformationProjectionMatrix(text_project * text_transform)
			.setColor(Magnum::Color3{1.0f})
			.setOutlineColor(Magnum::Color3{0.0f, 0.7f, 0.0f})
			.setOutlineRange(0.45f, 0.35f)
			.setSmoothness(0.025f);
		text_mesh.draw(text_shader);

		text_shader
			.setTransformationProjectionMatrix(text_project * Matrix3::translation(1.0f/text_project.rotationScaling().diagonal()))
			.setColor(Magnum::Color4{1.0f, 0.0f})
			.setOutlineRange(0.5f, 1.0f)
			.setSmoothness(0.075f);
		text_renderer->mesh().draw(text_shader);

		camera.draw(drawables);

		swapBuffers();
		redraw();
		timeline.nextFrame();
	}
	virtual void mousePressEvent(MouseEvent &e) override
	{
		if(e.button() != MouseEvent::Button::Left) return;

		previousMousePosition = e.position();
		e.setAccepted();
	}
	virtual void mouseMoveEvent(MouseMoveEvent &e) override
	{
		if(!(e.buttons() & MouseMoveEvent::Button::Left)) return;

		Vector2 delta
			= 3.0f
			* Vector2(e.position() - previousMousePosition)
			/ Vector2(Magnum::defaultFramebuffer.viewport().size());
		transformation
			= Matrix4::rotationX(Magnum::Rad(delta.y()))
			* transformation
			* Matrix4::rotationY(Magnum::Rad(delta.x()));

		previousMousePosition = e.position();
		e.setAccepted();
		redraw();
	}

	using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
	using Scene3D  = SceneGraph::Scene <SceneGraph::MatrixTransformation3D>;
	Scene3D scene;
	Object3D &camera_object {scene.addChild<Object3D>()};
	SceneGraph::Camera3D &camera {camera_object.addFeature<SceneGraph::Camera3D>()};
	SceneGraph::DrawableGroup3D drawables;

	Corrade::PluginManager::Manager<Text::AbstractFont> font_plugins;
	std::unique_ptr<Text::AbstractFont> font;
	Text::DistanceFieldGlyphCache glyph_cache;
	Magnum::Mesh text_mesh;
	std::unique_ptr<Text::Renderer2D> text_renderer;
	Magnum::Shaders::DistanceFieldVector2D text_shader;
	Matrix3 text_transform, text_project;

	Magnum::Timeline timeline;
	Magnum::Buffer indexBuffer, vertexBuffer;
	Magnum::Mesh mesh;
	Magnum::Shaders::Phong shader;

	Matrix4 transformation, projection;
	Vector2i previousMousePosition;
	Magnum::Color3 color;
};

MAGNUM_APPLICATION_MAIN(SimplePlatformer);
