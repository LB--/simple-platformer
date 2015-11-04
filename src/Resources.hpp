#ifndef simplat_Resources_HeaderPlusPlus
#define simplat_Resources_HeaderPlusPlus

#include <Magnum/Buffer.h>
#include <Magnum/Mesh.h>
#include <Magnum/ResourceManager.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/PhongMaterialData.h>

namespace simplat
{
	using ResourceManager = Magnum::ResourceManager
	<
		Magnum::Buffer,
		Magnum::Mesh,
		Magnum::Shaders::Phong,
		Magnum::Texture2D,
		Magnum::Trade::PhongMaterialData
	>;
}

#endif
