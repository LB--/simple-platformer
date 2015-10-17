#include <Magnum/Buffer.h>
#include <Magnum/Context.h>
#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Renderer.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Timeline.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Version.h>

struct SimplePlatformer final
: Magnum::Platform::Application
{
	explicit SimplePlatformer(Arguments const &arguments)
	: Magnum::Platform::Application{arguments, Configuration().setTitle("Simple Platformer").setSize({1280, 720})}
	{
		Magnum::Debug()
			<< "This application is running on"
			<< Magnum::Context::current()->version()
			<< "using"
			<< Magnum::Context::current()->rendererString();

		Magnum::Renderer::enable(Magnum::Renderer::Feature::DepthTest);
		Magnum::Renderer::enable(Magnum::Renderer::Feature::FaceCulling);

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

		transformation =
			Magnum::Matrix4::rotationX(Magnum::Deg(30.0f))*
			Magnum::Matrix4::rotationY(Magnum::Deg(40.0f));
		color = Magnum::Color3::fromHSV(Magnum::Deg(35.0f), 1.0f, 1.0f);

		projection =
			Magnum::Matrix4::perspectiveProjection(Magnum::Deg(35.0f), Magnum::Vector2(Magnum::defaultFramebuffer.viewport().size()).aspectRatio(), 0.01f, 100.0f)*
			Magnum::Matrix4::translation(Magnum::Vector3::zAxis(-10.0f));

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
	virtual void mouseReleaseEvent(MouseEvent &e) override
	{
		//color = Color3::fromHSV(color.hue() + Deg(50.0), 1.0f, 1.0f);

		e.setAccepted();
		redraw();
	}
	virtual void mouseMoveEvent(MouseMoveEvent &e) override
	{
		if(!(e.buttons() & MouseMoveEvent::Button::Left)) return;

		Magnum::Vector2 delta = 3.0f*Magnum::Vector2(e.position() - previousMousePosition)/Magnum::Vector2(Magnum::defaultFramebuffer.viewport().size());
		transformation =
			Magnum::Matrix4::rotationX(Magnum::Rad(delta.y()))*
			transformation*
			Magnum::Matrix4::rotationY(Magnum::Rad(delta.x()));

		previousMousePosition = e.position();
		e.setAccepted();
		redraw();
	}

	Magnum::Timeline timeline;
	Magnum::Buffer indexBuffer, vertexBuffer;
	Magnum::Mesh mesh;
	Magnum::Shaders::Phong shader;

	Magnum::Matrix4 transformation, projection;
	Magnum::Vector2i previousMousePosition;
	Magnum::Color3 color;
};

MAGNUM_APPLICATION_MAIN(SimplePlatformer);
