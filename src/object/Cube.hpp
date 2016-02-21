#ifndef simplat_object_Cube_HeaderPlusPlus
#define simplat_object_Cube_HeaderPlusPlus

#include <Magnum/Buffer.h>
#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Timeline.h>
#include <Magnum/Trade/MeshData3D.h>

#include <tuple>

namespace simplat { namespace object
{
	using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;
	using Magnum::SceneGraph::Drawable3D;
	struct Cube
	: Object3D
	, Drawable3D
	{
		Cube(Magnum::SceneGraph::DrawableGroup3D &group, Magnum::Timeline const &timeline, Object3D *parent)
		: Object3D{parent}
		, Drawable3D{*this, &group}
		, timeline(timeline)
		{
			Magnum::Trade::MeshData3D cube = Magnum::Primitives::Cube::solid();

			vertices.setData(Magnum::MeshTools::interleave(cube.positions(0), cube.normals(0)), Magnum::BufferUsage::StaticDraw);

			Magnum::Containers::Array<char> indexData;
			Magnum::Mesh::IndexType indexType;
			Magnum::UnsignedInt indexStart, indexEnd;
			std::tie(indexData, indexType, indexStart, indexEnd) = Magnum::MeshTools::compressIndices(cube.indices());
			indices.setData(indexData, Magnum::BufferUsage::StaticDraw);

			mesh
				.setPrimitive(cube.primitive())
				.setCount(cube.indices().size())
				.addVertexBuffer(vertices, 0, Magnum::Shaders::Phong::Position{}, Magnum::Shaders::Phong::Normal{})
				.setIndexBuffer(indices, 0, indexType, indexStart, indexEnd);

			shader
				.setLightPosition({7.0f, 5.0f, 2.5f})
				.setLightColor(Magnum::Color3(1.0f));
		}

	private:
		Magnum::Timeline const &timeline;
		Magnum::Color3 color = Magnum::Color3::fromHSV(Magnum::Deg(35.0f), 1.0f, 1.0f);
		Magnum::Mesh mesh;
		Magnum::Buffer vertices, indices;
		Magnum::Shaders::Phong shader;

		virtual void draw(Magnum::Matrix4 const &transmat, Magnum::SceneGraph::Camera3D &cam) override
		{
			color = Magnum::Color3::fromHSV(color.hue() + Magnum::Deg(90.0*timeline.previousFrameDuration()), 1.0f, 1.0f);
			shader
				.setDiffuseColor(color)
				.setAmbientColor(Magnum::Color3::fromHSV(color.hue(), 1.0f, 0.3f))
				.setTransformationMatrix(transmat)
				.setNormalMatrix(transmat.rotationScaling())
				.setProjectionMatrix(cam.projectionMatrix());
			mesh.draw(shader);
		}
	};
}}

#endif
