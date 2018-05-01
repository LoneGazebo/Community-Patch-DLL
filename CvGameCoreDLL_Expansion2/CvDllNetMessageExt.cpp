#include "CvGameCoreDLLPCH.h"
#include "CvDllNetMessageExt.h"

namespace NetMessageExt
{		
	// Nothing fancy now and should probably have some more machinery if there end up being more of these implemented.
	namespace Flags
	{
		enum FromDiplomacyFromUI
		{
			// Best not to use 255 since it could be confused with NO_PLAYER
			None = 0,
			DoEventChoice,
			DoCityEventChoice,	// Could use the same id but I doubt we will use more than 254		
		};
	}

	namespace Process
	{
		bool FromDiplomacyFromUI(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, FromUIDiploEventTypes eEvent, int iArg1, int iArg2) {
			Flags::FromDiplomacyFromUI flag = Flags::FromDiplomacyFromUI(static_cast<unsigned int>(eOtherPlayer) >> 24);
			switch (flag)
			{
				case Flags::None:
					return false;

				case Flags::DoEventChoice:				
				{
					PlayerTypes eActualPlayer = static_cast<PlayerTypes>(eOtherPlayer & 0xFF);
					EventTypes ePlayerEvent = static_cast<EventTypes>(eEvent);
					EventChoiceTypes eEventChoice = static_cast<EventChoiceTypes>(iArg2);
					Response::DoEventChoice(eActualPlayer, eEventChoice, ePlayerEvent);					
					break;
				}
				case Flags::DoCityEventChoice:
				{
					PlayerTypes eActualPlayer = static_cast<PlayerTypes>(eOtherPlayer & 0xFF);
					int iCityID = iArg1;
					CityEventTypes eCityEvent = static_cast<CityEventTypes>(eEvent);
					CityEventChoiceTypes eEventChoice = static_cast<CityEventChoiceTypes>(iArg2);
					Response::DoCityEventChoice(eActualPlayer, iCityID, eEventChoice, eCityEvent);
					break;
				}
			}
			return true;
		}
	}
	

	namespace Send
	{
		void DoEventChoice(PlayerTypes ePlayer, EventChoiceTypes eEventChoice, EventTypes eEvent)
		{
			CvAssertMsg((ePlayer & 0xFFFFFF00) == 0, "ePlayer representation outside of expected range");

			unsigned int uiMsgFlagAndPlayer = static_cast<unsigned int>(Flags::DoEventChoice << 24 | ePlayer);			
			gDLL->sendFromUIDiploEvent(static_cast<PlayerTypes>(uiMsgFlagAndPlayer), static_cast<FromUIDiploEventTypes>(eEvent), -1, static_cast<int>(eEventChoice));
		}
		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent)
		{
			CvAssertMsg((ePlayer & 0xFFFFFF00) == 0, "ePlayer representation outside of expected range");
			CvAssertMsg(iCityID >= 0, "iCityID outside of expected range");
			
			unsigned int uiMsgFlagAndPlayer = static_cast<unsigned int>(Flags::DoCityEventChoice << 24 | ePlayer);
			gDLL->sendFromUIDiploEvent(static_cast<PlayerTypes>(uiMsgFlagAndPlayer), static_cast<FromUIDiploEventTypes>(eCityEvent), iCityID, static_cast<int>(eEventChoice));
		}
	}

	namespace Response
	{

		void DoEventChoice(PlayerTypes ePlayer, EventChoiceTypes eEventChoice, EventTypes eEvent)
		{
			CvPlayer& kActualPlayer = GET_PLAYER(ePlayer);
			kActualPlayer.DoEventChoice(eEventChoice, eEvent, false);
		}

		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent)
		{
			CvPlayer& kActualPlayer = GET_PLAYER(ePlayer);
			int iLoop;
			CvCity* pLoopCity;
			for (pLoopCity = kActualPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kActualPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->GetID() == iCityID)
				{
					pLoopCity->DoEventChoice(eEventChoice, eCityEvent, false);
					break;
				}

			}
		}
	}
}