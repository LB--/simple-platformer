#include "config.h"

#include "object/Cube.hpp"
#include "ui/Framerate.hpp"
#include "ui/Title.hpp"

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/Context.h>
#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Renderer.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Timeline.h>
#include <Magnum/Version.h>

#include <cstdlib>
#include <functional>
#include <memory>
#include <string>

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
				.setWindowFlags(Configuration::WindowFlag::Resizable)
		}
	, font_plugins{MAGNUM_PLUGINS_FONT_DIR}
	, glyph_cache{Vector2i{2048}, Vector2i{512}, 22}
	{
		Magnum::Debug()
			<< "This application is running on"
			<< Magnum::Context::current().version()
			<< "using"
			<< Magnum::Context::current().rendererString();

//		Renderer::enable(Renderer::Feature::DepthTest); //breaks text rendering
		Renderer::enable(Renderer::Feature::FaceCulling);
		Renderer::enable(Renderer::Feature::Blending);
		Renderer::setBlendFunction(Renderer::BlendFunction::SourceAlpha, Renderer::BlendFunction::OneMinusSourceAlpha);
		Renderer::setBlendEquation(BlendEquation::Add, BlendEquation::Add);

		//load the font
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

		//set up the UI
		ui.addChild<simplat::ui::Title>(std::ref(ui_drawables), std::ref(*font), std::ref(glyph_cache));
		ui.addChild<simplat::ui::Framerate>(std::ref(ui_drawables), std::cref(tps), std::ref(*font), std::cref(glyph_cache));
		ui.addChild<simplat::ui::Framerate>(std::ref(ui_drawables), std::cref(fps), std::ref(*font), std::cref(glyph_cache))
			.setTransformation(Matrix3::translation(Vector2::yAxis(-0.05f))); //TODO

		//angle the cube
		transformation
			= Matrix4::rotationX(Magnum::Deg(30.0f))
			* Matrix4::rotationY(Magnum::Deg(40.0f));
		cube.setTransformation(transformation);


		//set up the 3D camera
		camera_object.translate(Vector3::zAxis(5.0f));
		camera
			.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
			.setViewport(Magnum::defaultFramebuffer.viewport().size());
		camera.setProjectionMatrix(Matrix4::perspectiveProjection
			(
				Magnum::Deg(35.0f),
				Vector2(Magnum::defaultFramebuffer.viewport().size()).aspectRatio(),
				0.01f,
				100.0f
			)
			* Matrix4::translation(Vector3::zAxis(-10.0f))
		);

		//set up the UI camera
		ui_camera
			.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
			.setProjectionMatrix(Matrix3::projection({16.0f/9.0f, 1.0f}))
			.setViewport(Magnum::defaultFramebuffer.viewport().size());

		setSwapInterval(1); //enable VSync
		tps.start();
		fps.start();
	}

private:
	virtual void viewportEvent(Vector2i const &size) override
	{
		Magnum::defaultFramebuffer.setViewport(Magnum::Range2Di{Vector2i{}, size});
		camera.setViewport(size);
		ui_camera.setViewport(size);
	}
	virtual void tickEvent() override
	{
		tps.nextFrame();
	}
	virtual void drawEvent() override
	{
		Magnum::defaultFramebuffer.clear(Magnum::FramebufferClear::Color|Magnum::FramebufferClear::Depth);

		camera.draw(drawables);
		ui_camera.draw(ui_drawables);

		swapBuffers();
		redraw();
		fps.nextFrame();
	}
	virtual void mousePressEvent(MouseEvent &e) override
	{
		if(e.button() != MouseEvent::Button::Left) return;

		previousMousePosition = e.position();
		e.setAccepted();
	}
	virtual void mouseReleaseEvent(MouseEvent &e) override
	{
		//
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
		cube.setTransformation(transformation);

		previousMousePosition = e.position();
		e.setAccepted();
		redraw();
	}
	virtual void keyPressEvent(KeyEvent &e) override
	{
		if(e.key() == KeyEvent::Key::Esc)
		{
			e.setAccepted();
			exit();
		}
	}
	virtual void keyReleaseEvent(KeyEvent &e) override
	{
		//
	}

	Magnum::Timeline tps, fps;

	using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
	using Scene3D  = SceneGraph::Scene <SceneGraph::MatrixTransformation3D>;
	Scene3D scene;
	Object3D &camera_object {scene.addChild<Object3D>()};
	SceneGraph::Camera3D &camera {camera_object.addFeature<SceneGraph::Camera3D>()};
	SceneGraph::DrawableGroup3D drawables;
	simplat::object::Cube &cube {scene.addChild<simplat::object::Cube>(std::ref(drawables), std::cref(tps))};

	Corrade::PluginManager::Manager<Text::AbstractFont> font_plugins;
	std::unique_ptr<Text::AbstractFont> font;
	Text::DistanceFieldGlyphCache glyph_cache;

	using Object2D = SceneGraph::Object<SceneGraph::MatrixTransformation2D>;
	using Scene2D  = SceneGraph::Scene <SceneGraph::MatrixTransformation2D>;
	Scene2D ui;
	Object2D &ui_camera_object {ui.addChild<Object2D>()};
	SceneGraph::Camera2D &ui_camera {ui_camera_object.addFeature<SceneGraph::Camera2D>()};
	SceneGraph::DrawableGroup2D ui_drawables;

	Matrix4 transformation;
	Vector2i previousMousePosition;
};

MAGNUM_APPLICATION_MAIN(SimplePlatformer);
