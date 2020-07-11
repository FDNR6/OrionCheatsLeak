#pragma once

namespace AimBot
{
	bool Active = false;
	bool Silent = false;
	bool BestDistance = false;
	bool CloseToCrosshair = true;
	bool AutoSwitch = true;
	float FOV = 25.f;
	float Smooth = 5.f;
}

namespace Visuals
{
	bool ESP = true;
	bool Distance = true;
	bool Skeleton = true;
	bool Head = true;
	bool Crosshair = true;
	bool FOV = true;
}

namespace Misc
{
	bool NoRecoil = false;
	bool NoSpread = false;
	bool NoFlash = false;
}