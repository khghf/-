#pragma once
UENUM()
enum class EWarriorConfirmType :uint8
{
	Yes,
	No,
};
UENUM()
enum class EWarriorValidType :uint8
{
	Valid,
	InValid,
};
UENUM()
enum class EWarriorSuccessType :uint8
{
	Successful,
	Failed,
};
UENUM()
enum class EWarriorCountDownActionInput :uint8
{
	Start,
	Cancel,
	RefreshNow,//立即完成冷却
};
UENUM()
enum class EWarriorCountDownActionOutInput :uint8
{
	Update,
	Completed,
	Cancelled, 
};
UENUM(BlueprintType)
enum class EWarriorGameDifficulty :uint8
{
	Easy,
	Normal,
	Hard,
	VeryHard,
};
UENUM(BlueprintType)
enum class EWarriorInputMode :uint8
{
	GameOnly,
	UIOnly,
};
//动画
UENUM(BlueprintType)
enum class ECardinalDirection :uint8
{
	Forward,
	Backward,
	Left,
	Right,
};
UENUM(BlueprintType)
enum class ERootYawOffsetMode :uint8
{
	BlendOut,
	Hold,
	Accumulate,
};