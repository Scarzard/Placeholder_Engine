#include "ResourceAnimation.h"

bool Channel::ActivePK() const
{
	return ((PositionKeys.size() == 1 && PositionKeys.begin()->first == -1) == false);
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

bool Channel::ActiveRK() const
{
	return ((RotationKeys.size() == 1 && RotationKeys.begin()->first == -1) == false);
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

bool Channel::ActiveSK() const
{
	return ((ScaleKeys.size() == 1 && ScaleKeys.begin()->first == -1) == false);
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