#include <string.h>
#include <wchar.h>

#include "frd.h"

static Handle frdHandle;
static int frdRefCount;

Result frdInit(void)
{
	Result ret = 0;

	if (AtomicPostIncrement(&frdRefCount)) 
		return 0;

	ret = srvGetServiceHandle(&frdHandle, "frd:u");
	
	if (R_FAILED(ret)) 
		ret = srvGetServiceHandle(&frdHandle, "frd:n");
	
	if (R_FAILED(ret)) 
		ret = srvGetServiceHandle(&frdHandle, "frd:a");
	
	if (R_FAILED(ret)) 
		AtomicDecrement(&frdRefCount);

	return ret;
}

void frdExit(void)
{
	if (AtomicDecrement(&frdRefCount)) 
		return;
	
	svcCloseHandle(frdHandle);
}

Result FRDU_IsOnline(bool * state) // Online state
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x02, 0, 0); // 0x00020000

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	*state = cmdbuf[2] & 0xFF;
	
	return cmdbuf[1];
}

Result FRDU_HasLoggedIn(bool * state)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x01, 0, 0); // 0x00010000

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	*state = cmdbuf[2] & 0xFF;
	
	return cmdbuf[1];
}

Result FRD_Login(Handle event)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x03, 0, 2); // 0x00030002
	cmdbuf[1] = 0;
	cmdbuf[2] = event;
    
	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	return cmdbuf[1];
}

Result FRD_Logout(void)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x04, 0, 0); // 0x00040000
    
	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	return cmdbuf[1];
}

Result FRD_GetMyFriendKey(FriendKey * key)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x05, 0, 0); // 0x00050000

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	memcpy(key, &cmdbuf[2], sizeof(FriendKey));
	
	return cmdbuf[1];
}

Result FRD_GetMyPreference(bool * isPublicMode, bool * isShowGameName, bool * isShowPlayedGame)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x06, 0, 0); // 0x00060000

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	*isPublicMode = cmdbuf[2] & 0xFF; // Public mode 
	*isShowGameName = cmdbuf[3] & 0xFF; // Show current game 
	*isShowPlayedGame = cmdbuf[4] & 0xFF; // Show game history.
	
	return cmdbuf[1];
}

Result FRD_GetMyProfile(Profile * profile)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x07, 0, 0); // 0x70000

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	memcpy(profile, &cmdbuf[2], sizeof(Profile));
	
	return cmdbuf[1];
}

Result FRD_GetMyScreenName(u16 * name)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x09, 0, 0); // 0x00090000

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;
	
	memcpy(name, &cmdbuf[2], FRIENDS_SCREEN_NAME_SIZE); // 11-byte UTF-16 screen name (with null terminator)
	
	return cmdbuf[1];
}

Result FRD_GetMyMii(miiStoreData * mii)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x0A, 0, 0); // 0x000A0000

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	memcpy(mii, &cmdbuf[2], FRIEND_MII_STORE_DATA_SIZE);
	
	return cmdbuf[1];
}

Result FRD_GetMyComment(u16 * comment)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x0F, 0, 0); // 0x000F0000

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;
	
	memcpy(comment, &cmdbuf[2], FRIENDS_COMMENT_SIZE); // 16-byte UTF-16 comment
	
	return cmdbuf[1];
}

Result FRD_GetMyPlayingGame(u64 * titleId)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x0C, 0, 0); // 0x000C0000

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	*titleId = (((u64)cmdbuf[3]) << 32 | (u64)cmdbuf[2]);
	
	return cmdbuf[1];
}

Result FRD_GetMyFavoriteGame(u64 * titleId)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x0D, 0, 0); // 0x000D0000

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	*titleId = (((u64)cmdbuf[3]) << 32 | (u64)cmdbuf[2]);
	
	return cmdbuf[1];
}

Result FRD_UpdateGameModeDescription(const wchar_t * desc)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x1D, 0, 2); // 0x001D0002
    cmdbuf[1] = 0x400802;
    cmdbuf[2] = (uintptr_t)desc;
    
    if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	return cmdbuf[1];
}

Result FRD_PrincipalIdToFriendCode(u32 principalId, u64 * friendCode)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x24, 1, 0); // 0x00240040
	cmdbuf[1] = principalId;

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	*friendCode = (((u64)cmdbuf[3]) << 32 | (u64)cmdbuf[2]);
	
	return cmdbuf[1];
}

Result FRD_FriendCodeToPrincipalId(u64 friendCode, u32 * principalId)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x25, 2, 0); // 0x00250080
	cmdbuf[1] = (u32)(friendCode & 0xFFFFFFFF);
	cmdbuf[2] = (u32)(friendCode >> 32);

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	*principalId = cmdbuf[2];
	
	return cmdbuf[1];
}

// Result FRD_GetFriendKeyList(FriendKey * friendKeyList, size_t * num, size_t offset, size_t size);

Result FRD_GetFriendProfile(Profile * profileList, const FriendKey * friendKeyList, size_t size)
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x15, 1, 2); // 0x00150042
	cmdbuf[1] = size;
	cmdbuf[2] = (size << 18) | 2;
	cmdbuf[3] = (u32)friendKeyList;
	cmdbuf[4] = ((size * sizeof(Profile)) << 10) | 2;
	cmdbuf[5] = (u32)profileList;

	if (R_FAILED(ret = svcSendSyncRequest(frdHandle)))
		return ret;

	return cmdbuf[1];
}