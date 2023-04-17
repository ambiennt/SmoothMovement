#include "main.h"
#include <dllentry.h>

void dllenter() {}
void dllexit() {}

// disable Dimension::sendPacketForEntity call for MovePlayerPacket
// patch ServerNetworkHandler opcodes
void PreInit() {
	const char *symbol = "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVMovePlayerPacket@@@Z";
	GetServerSymbolWithOffset<PatchSpan<7>>(symbol, 0x281)->VerifyPatchFunction(
		{0x48, 0x8B, 0x8B, 0x28, 0x03, 0x00, 0x00}, NopFilled{}); // 48 8B 8B 28 03 00 00
	GetServerSymbolWithOffset<PatchSpan<5>>(symbol, 0x288)->VerifyPatchFunction(
		{0xE8, 0xD3, 0xEA, 0x6B, 0x00}, NopFilled{}); // E8 D3 EA 6B 00
}
void PostInit() {}

TInstanceHook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVMovePlayerPacket@@@Z",
	ServerNetworkHandler, const NetworkIdentifier &netId, const MovePlayerPacket &oldPkt) {

	original(this, netId, oldPkt);
	auto player = this->getServerPlayer(netId, oldPkt.mClientSubId);
	if (!player) return;


	// todo make sure the clamping for values is done BEFORE dispatching the MoveActorAbsolutePacket
	// just rewrite this whole func and merge the AmbientAC clamp funcs

	constexpr double degreeToByteRatio = 360.0 / 256.0;
	MoveActorAbsolutePacket newPkt{};
	newPkt.mMoveData.mRuntimeId         = oldPkt.mRuntimeId;
	newPkt.mMoveData.mFlags.mIsOnGround = static_cast<uint8_t>(oldPkt.mOnGround);

	// setting this flag causes some weird rubber banding both in players and mobs when they get teleported
	//newPkt.mMoveData.mFlags.mTeleported = static_cast<uint8_t>(oldPkt.mPositionMode == Player::PositionMode::TELEPORT);
	newPkt.mMoveData.mPos               = oldPkt.mPos;
	newPkt.mMoveData.mRotX              = static_cast<uint8_t>(static_cast<double>(oldPkt.mRot.x) / degreeToByteRatio);
	newPkt.mMoveData.mRotY              = static_cast<uint8_t>(static_cast<double>(oldPkt.mRot.y) / degreeToByteRatio);

	// sometimes causes weird head spazzes? cant pinpoint the exact cause
	//newPkt.mMoveData.mRotYHead          = static_cast<uint8_t>(static_cast<double>(oldPkt.mRotYHead) / degreeToByteRatio5);
	newPkt.mMoveData.mRotYHead          = static_cast<uint8_t>(static_cast<double>(oldPkt.mRot.y) / degreeToByteRatio);

	player->mDimension->sendPacketForEntity(*player, newPkt, player);
}