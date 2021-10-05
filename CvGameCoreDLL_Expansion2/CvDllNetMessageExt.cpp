#include "CvGameCoreDLLPCH.h"
#include "CvDllNetMessageExt.h"

namespace NetMessageExt
{		
	namespace Process
	{
		bool ResponseMoveGreatWorks(PlayerTypes ePlayer, int iArg1, int iArg2, int iArg3, int iArg4, int iArg5, int iArg6) {
			switch (iArg6)
			{
				case 0:
					return false;

				case 1:				
				{
					PlayerTypes eActualPlayer = static_cast<PlayerTypes>(ePlayer);
					EventTypes ePlayerEvent = static_cast<EventTypes>(iArg1);
					EventChoiceTypes eEventChoice = static_cast<EventChoiceTypes>(iArg3);
					Response::DoEventChoice(eActualPlayer, eEventChoice, ePlayerEvent);					
					break;
				}
				case 2:
				{
					PlayerTypes eActualPlayer = static_cast<PlayerTypes>(ePlayer);
					CityEventTypes eCityEvent = static_cast<CityEventTypes>(iArg1);
					int iCityID = iArg2;
					CityEventChoiceTypes eEventChoice = static_cast<CityEventChoiceTypes>(iArg3);
					int iSpyID = iArg4;
					PlayerTypes eSpyOwner = static_cast<PlayerTypes>(iArg5);
					Response::DoCityEventChoice(eActualPlayer, iCityID, eEventChoice, eCityEvent, iSpyID, eSpyOwner);
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
			gDLL->sendMoveGreatWorks(static_cast<PlayerTypes>(ePlayer), static_cast<int>(eEvent), -1, static_cast<int>(eEventChoice), -1, -1, 1);
		}
		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent, int iSpyID, PlayerTypes eSpyOwner)
		{			
			gDLL->sendMoveGreatWorks(static_cast<PlayerTypes>(ePlayer), static_cast<int>(eCityEvent), iCityID, static_cast<int>(eEventChoice), iSpyID, static_cast<PlayerTypes>(eSpyOwner), 2);
		}
	}

	namespace Response
	{

		void DoEventChoice(PlayerTypes ePlayer, EventChoiceTypes eEventChoice, EventTypes eEvent)
		{
			CvPlayer& kActualPlayer = GET_PLAYER(ePlayer);
			kActualPlayer.DoEventChoice(eEventChoice, eEvent, false);
		}

		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent, int iEspionageValue, PlayerTypes eSpyOwner)
		{
			CvPlayer& kActualPlayer = GET_PLAYER(ePlayer);
			int iLoop;
			CvCity* pLoopCity;
			for (pLoopCity = kActualPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kActualPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->GetID() == iCityID)
				{
					pLoopCity->DoEventChoice(eEventChoice, eCityEvent, false, iEspionageValue, eSpyOwner);
					break;
				}

			}
		}
	}
}