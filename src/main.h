#pragma once

#include <hook.h>
#include <base/base.h>
#include <base/log.h>
#include <Level/Dimension.h>
#include <Net/NetworkIdentifier.h>
#include <Net/ServerNetworkHandler.h>
#include <Actor/ActorRuntimeID.h>
#include <Actor/Player.h>
#include <Actor/ServerPlayer.h>
#include <Packet/MoveActorAbsolutePacket.h>
#include <Packet/MovePlayerPacket.h>
#include <Packet/AddPlayerPacket.h>
#include <Packet/RemoveActorPacket.h>

DEF_LOGGER("SmoothMovement");