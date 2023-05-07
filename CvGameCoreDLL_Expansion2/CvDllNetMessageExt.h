#pragma once

namespace NetMessageExt
{
	// Used in CvDllNetMessageHandler response functions to detect and translate message to the correct Response function instead of executing the normal handling code
	// All return true if the message was detected and the calling function should return immediately if that is the case.
	namespace Process
	{
		bool ResponseMoveGreatWorks(PlayerTypes ePlayer, int iArg1, int iArg2, int iArg3, int iArg4, int iArg5, int iArg6);
	}

	// Used to send messages by encoding values so that they can safely be transmitted via DLL calls that were designed for a different purpose
	namespace Send
	{
		void DoEventChoice(PlayerTypes ePlayer, EventChoiceTypes eEventChoice, EventTypes eEvent);
		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent, int iSpyID, PlayerTypes eSpyOwner);
		void DoMinorBullyAnnex(PlayerTypes ePlayer, PlayerTypes eMinor);
		void DoMinorBuyout(PlayerTypes ePlayer, PlayerTypes eMinor);
		//void RefreshTradeRouteCache(PlayerTypes ePlayer);
	}

	// Used to respond to sent messages in the normal fashion
	namespace Response
	{
		void DoEventChoice(PlayerTypes ePlayer, EventChoiceTypes eEventChoice, EventTypes eEvent);
		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent, int iSpyID, PlayerTypes eSpyOwner);
		void DoMinorBullyAnnex(PlayerTypes ePlayer, PlayerTypes eMinor);
		void DoMinorBuyout(PlayerTypes ePlayer, PlayerTypes eMinor);
		//void RefreshTradeRouteCache(PlayerTypes ePlayer);
	}
}

