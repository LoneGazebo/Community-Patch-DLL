#include "CvGameCoreDLLPCH.h"
#include "CvDllNetMessageExt.h"

namespace NetMessageExt
{		
	namespace Process
	{
		bool ResponseMoveGreatWorks(PlayerTypes ePlayer, int iArg1, int iArg2, int iArg3, int iArg4, int iArg5, int iArg6) {
			if (iArg6 < 999)
				return false;
			switch (iArg6)
			{
				case 999:				
				{
					PlayerTypes const eActualPlayer = static_cast<PlayerTypes>(ePlayer);
					EventTypes const ePlayerEvent = static_cast<EventTypes>(iArg1);
					EventChoiceTypes const eEventChoice = static_cast<EventChoiceTypes>(iArg3);
					Response::DoEventChoice(eActualPlayer, eEventChoice, ePlayerEvent);					
					break;
				}
				case 1000:
				{
					PlayerTypes const eActualPlayer = static_cast<PlayerTypes>(ePlayer);
					CityEventTypes const eCityEvent = static_cast<CityEventTypes>(iArg1);
					int const iCityID = iArg2;
					CityEventChoiceTypes const eEventChoice = static_cast<CityEventChoiceTypes>(iArg3);
					int const iSpyID = iArg4;
					PlayerTypes const eSpyOwner = static_cast<PlayerTypes>(iArg5);
					Response::DoCityEventChoice(eActualPlayer, iCityID, eEventChoice, eCityEvent, iSpyID, eSpyOwner);
					break;
				}
				case 1001:
				{
					PlayerTypes const eActualPlayer = static_cast<PlayerTypes>(ePlayer);
					PlayerTypes const eMinor = static_cast<PlayerTypes>(iArg1);
					Response::DoMinorBullyAnnex(eActualPlayer, eMinor);
					break;
				}
				case 1002:
				{
					PlayerTypes const eActualPlayer = static_cast<PlayerTypes>(ePlayer);
					PlayerTypes const eMinor = static_cast<PlayerTypes>(iArg1);
					Response::DoMinorBuyout(eActualPlayer, eMinor);
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
			gDLL->sendMoveGreatWorks(static_cast<PlayerTypes>(ePlayer), static_cast<int>(eEvent), -1, static_cast<int>(eEventChoice), -1, -1, 999);
		}
		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent, int iSpyID, PlayerTypes eSpyOwner)
		{			
			gDLL->sendMoveGreatWorks(static_cast<PlayerTypes>(ePlayer), static_cast<int>(eCityEvent), iCityID, static_cast<int>(eEventChoice), iSpyID, static_cast<PlayerTypes>(eSpyOwner), 1000);
		}
		void DoMinorBullyAnnex(PlayerTypes ePlayer, PlayerTypes eMinor)
		{	
			gDLL->sendMoveGreatWorks(static_cast<PlayerTypes>(ePlayer), static_cast<PlayerTypes>(eMinor), -1, -1, -1, -1, 1001);
		}
		void DoMinorBuyout(PlayerTypes ePlayer, PlayerTypes eMinor)
		{	
			gDLL->sendMoveGreatWorks(static_cast<PlayerTypes>(ePlayer), static_cast<PlayerTypes>(eMinor), -1, -1, -1, -1, 1002);
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
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
			for (pLoopCity = kActualPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kActualPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->GetID() == iCityID)
				{
					pLoopCity->DoEventChoice(eEventChoice, eCityEvent, false, iEspionageValue, eSpyOwner);
					break;
				}

			}
		}
		void DoMinorBullyAnnex(PlayerTypes ePlayer, PlayerTypes eMinor)
		{	
			GET_PLAYER(eMinor).GetMinorCivAI()->DoMajorBullyAnnex(ePlayer);
		}
		void DoMinorBuyout(PlayerTypes ePlayer, PlayerTypes eMinor)
		{	
			GET_PLAYER(eMinor).GetMinorCivAI()->DoBuyout(ePlayer);
		}
	}
}