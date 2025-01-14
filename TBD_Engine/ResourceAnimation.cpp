#include "ResourceAnimation.h"
#include "Application.h"
#include "MeshLoader.h"
#include "ModuleResources.h"

#include "mmgr/mmgr.h"

bool Channel::PosHasKey() const
{
	return !PositionKeys.empty();
}

std::map<double, float3>::iterator Channel::PrevPosition(double current)
{
	std::map<double, float3>::iterator ret = PositionKeys.lower_bound(current);
	if (ret != PositionKeys.begin())
		ret--;

	return ret;
}

std::map<double, float3>::iterator Channel::NextPosition(double current)
{
	return PositionKeys.upper_bound(current);
}

bool Channel::RotHasKey() const
{
	return !RotationKeys.empty();
}

std::map<double, Quat>::iterator Channel::PrevRotation(double current )
{
	std::map<double, Quat>::iterator ret = RotationKeys.lower_bound(current);
	if (ret != RotationKeys.begin())
		ret--;
	return ret;
}

std::map<double, Quat>::iterator Channel::NextRotation(double current)
{
	return RotationKeys.upper_bound(current);
}

bool Channel::ScaleHasKey() const
{
	return !ScaleKeys.empty();
}

std::map<double, float3>::iterator Channel::PrevScale(double current)
{
	std::map<double, float3>::iterator ret = ScaleKeys.lower_bound(current);
	if (ret != ScaleKeys.begin())
		ret--;
	return ret;
}

std::map<double, float3>::iterator Channel::NextScale(double current)
{
	return ScaleKeys.upper_bound(current);
}

bool ResourceAnimation::LoadInMemory()
{
	return App->mesh_loader->LoadAnim(this);
}

void ResourceAnimation::ReleaseMemory()
{
	std::map<uint, Resource*>::const_iterator it = App->resources->resources.find(this->res_UUID);

	if (it != App->resources->resources.end())
		App->resources->resources.erase(it);

	delete[] this->channels;
	this->channels = nullptr;


	delete this;
}
