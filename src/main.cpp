#include "Hooks.h"
#include "ModConfigUI.h"
#include "Settings.h"

#include "Version.h"

static void MessageHandler(SKSE::MessagingInterface::Message* a_message)
{
	if (a_message->type == SKSE::MessagingInterface::kPostLoad)
	{
		DME::InstallModConfigUI();
	}
}

extern "C"
{
	DLLEXPORT bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = Version::NAME.data();
		a_info->version = REL::Version{ Version::MAJOR, Version::MINOR, Version::PATCH, 0 }.pack();

		if (a_skse->IsEditor())
		{
			logger::critical("Loaded in editor, marking as incompatible"sv);
			return false;
		}

		const auto ver = a_skse->RuntimeVersion();
		if (ver < SKSE::RUNTIME_SSE_1_5_39)
		{
			logger::critical("Unsupported runtime version {}", ver.string());
			return false;
		}

		return true;
	}

	DLLEXPORT bool SKSEPlugin_Load(SKSE::LoadInterface* a_skse)
	{
		SKSE::InitInfo initInfo{};
		initInfo.logLevel = REX::ELogLevel::Trace;
		initInfo.logPattern = "%s(%#): [%^%l%$] %v";
		initInfo.trampoline = true;
		initInfo.trampolineSize = 1 << 6;
		SKSE::Init(a_skse, initInfo);

		logger::info("{} v{} -({})", Version::FORMATTED_NAME, Version::STRING, __TIMESTAMP__);

		const SKSE::MessagingInterface* messaging = SKSE::GetMessagingInterface();
		if (messaging->RegisterListener("SKSE", MessageHandler))
		{
			logger::info("Messaging interface registration successful.");
		}
		else
		{
			logger::critical("Messaging interface registration failed.");
			return false;
		}

		DME::LoadSettings();
		logger::info("Settings loaded.");

		DME::InstallHooks();
		logger::info("Hooks installed.");

		logger::info("Dialogue Movement Enabler loaded.");

		return true;
	}
};
