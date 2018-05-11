#pragma once

namespace NetMessageExt
{
	// Used in CvDllNetMessageHandler response functions to detect and translate message to the correct Response function instead of executing the normal handling code
	// All return true if the message was detected and the calling function should return immediately if that is the case.
	namespace Process
	{
		bool FromDiplomacyFromUI(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, FromUIDiploEventTypes eEvent, int iArg1, int iArg2);
	}

	// Used to send messages by encoding values so that they can safely be transmitted via DLL calls that were designed for a different purpose
	namespace Send
	{
		void DoEventChoice(PlayerTypes ePlayer, EventChoiceTypes eEventChoice, EventTypes eEvent);
		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent);
		void RefreshTradeRouteCache(PlayerTypes ePlayer);
	}

	// Used to respond to sent messages in the normal fashion
	namespace Response
	{
		void DoEventChoice(PlayerTypes ePlayer, EventChoiceTypes eEventChoice, EventTypes eEvent);
		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent);
		void RefreshTradeRouteCache(PlayerTypes ePlayer);
	}
}

