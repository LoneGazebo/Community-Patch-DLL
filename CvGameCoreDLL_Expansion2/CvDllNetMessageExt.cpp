#include "CvGameCoreDLLPCH.h"
#include "CvDllNetMessageExt.h"
#include "CvDiplomacyAI.h"

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
					PlayerTypes eActualPlayer = ePlayer;
					EventTypes ePlayerEvent = static_cast<EventTypes>(iArg1);
					EventChoiceTypes eEventChoice = static_cast<EventChoiceTypes>(iArg3);
					Response::DoEventChoice(eActualPlayer, eEventChoice, ePlayerEvent);					
					break;
				}
				case 1000:
				{
					PlayerTypes eActualPlayer = ePlayer;
					CityEventTypes eCityEvent = static_cast<CityEventTypes>(iArg1);
					int iCityID = iArg2;
					CityEventChoiceTypes eEventChoice = static_cast<CityEventChoiceTypes>(iArg3);
					int iSpyID = iArg4;
					PlayerTypes eSpyOwner = static_cast<PlayerTypes>(iArg5);
					Response::DoCityEventChoice(eActualPlayer, iCityID, eEventChoice, eCityEvent, iSpyID, eSpyOwner);
					break;
				}
				case 1001:
				{
					PlayerTypes eActualPlayer = ePlayer;
					PlayerTypes eMinor = static_cast<PlayerTypes>(iArg1);
					Response::DoMinorBullyAnnex(eActualPlayer, eMinor);
					break;
				}
				case 1002:
				{
					PlayerTypes eActualPlayer = ePlayer;
					PlayerTypes eMinor = static_cast<PlayerTypes>(iArg1);
					bool booleanFromInt = false;
					booleanFromInt = iArg2 == 1;
					Response::DoQuestInfluenceDisabled(eActualPlayer, eMinor, booleanFromInt);
					break;
				}
				case 1003:
				{
					PlayerTypes eActualPlayer = ePlayer;
					bool bValue = iArg1 == 1;
					Response::SetDisableAutomaticFaithPurchase(eActualPlayer, bValue);
					break;
				}
				case 1004:
				{
					TeamTypes eMaster = static_cast<TeamTypes>(iArg1);
					PlayerTypes eVassal = static_cast<PlayerTypes>(iArg2);
					int iPercent = iArg3;
					Response::DoApplyVassalTax(eMaster, eVassal, iPercent);
					break;
				}
				case 1005:
				{
					TeamTypes eMaster = static_cast<TeamTypes>(iArg1);
					TeamTypes eVassal = static_cast<TeamTypes>(iArg2);
					Response::DoLiberateVassal(eMaster, eVassal);
					break;
				}
				case 1006:
				{
					PlayerTypes ePlayer1 = static_cast<PlayerTypes>(iArg1);
					PlayerTypes ePlayer2 = static_cast<PlayerTypes>(iArg2);
					Response::DoClearDealRenewalID(ePlayer1, ePlayer2);
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
			gDLL->sendMoveGreatWorks(ePlayer, static_cast<int>(eEvent), -1, static_cast<int>(eEventChoice), -1, -1, 999);
		}
		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent, int iSpyID, PlayerTypes eSpyOwner)
		{			
			gDLL->sendMoveGreatWorks(ePlayer, static_cast<int>(eCityEvent), iCityID, static_cast<int>(eEventChoice), iSpyID, eSpyOwner, 1000);
		}
		void DoMinorBullyAnnex(PlayerTypes ePlayer, PlayerTypes eMinor)
		{	
			gDLL->sendMoveGreatWorks(ePlayer, eMinor, -1, -1, -1, -1, 1001);
		}
		void DoQuestInfluenceDisabled(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue)
		{
			int booleanToInt = 0;
			if (bValue) {
				booleanToInt = 1;
			} else {
				booleanToInt = 0;
			}
			gDLL->sendMoveGreatWorks(ePlayer, eMinor, booleanToInt, -1, -1, -1, 1002);
		}
		void SetDisableAutomaticFaithPurchase(PlayerTypes ePlayer, bool bValue)
		{
			int booleanToInt = bValue ? 1 : 0;
			gDLL->sendMoveGreatWorks(ePlayer, booleanToInt, -1, -1, -1, -1, 1003);
		}
		void DoApplyVassalTax(TeamTypes eMaster, PlayerTypes eVassal, int iPercent)
		{
			PlayerTypes eMasterPlayer = GET_TEAM(eMaster).getLeaderID();
			gDLL->sendMoveGreatWorks(eMasterPlayer, static_cast<int>(eMaster), static_cast<int>(eVassal), iPercent, -1, -1, 1004);
		}
		void DoLiberateVassal(TeamTypes eMaster, TeamTypes eVassal)
		{
			PlayerTypes eMasterPlayer = GET_TEAM(eMaster).getLeaderID();
			gDLL->sendMoveGreatWorks(eMasterPlayer, static_cast<int>(eMaster), static_cast<int>(eVassal), -1, -1, -1, 1005);
		}
		void DoClearDealRenewalID(PlayerTypes ePlayer1, PlayerTypes ePlayer2)
		{
			gDLL->sendMoveGreatWorks(ePlayer1, static_cast<int>(ePlayer1), static_cast<int>(ePlayer2), -1, -1, -1, 1006);
		}
	}

	namespace Response
	{

		void DoEventChoice(PlayerTypes ePlayer, EventChoiceTypes eEventChoice, EventTypes eEvent)
		{
			CvPlayer& kActualPlayer = GET_PLAYER(ePlayer);
			kActualPlayer.DoEventChoice(eEventChoice, eEvent, false);
		}

		void DoCityEventChoice(PlayerTypes ePlayer, int iCityID, CityEventChoiceTypes eEventChoice, CityEventTypes eCityEvent, int iSpyID, PlayerTypes eSpyOwner)
		{
			CvPlayer& kActualPlayer = GET_PLAYER(ePlayer);
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
			for (pLoopCity = kActualPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kActualPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->GetID() == iCityID)
				{
					pLoopCity->DoEventChoice(eEventChoice, eCityEvent, false, iSpyID, eSpyOwner);
					break;
				}

			}
		}
		void DoMinorBullyAnnex(PlayerTypes ePlayer, PlayerTypes eMinor)
		{	
			GET_PLAYER(eMinor).GetMinorCivAI()->DoMajorBullyAnnex(ePlayer);
		}
		void DoQuestInfluenceDisabled(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue)
		{
			GET_PLAYER(eMinor).GetMinorCivAI()->SetQuestInfluenceDisabled(ePlayer, bValue);
		}
		void SetDisableAutomaticFaithPurchase(PlayerTypes ePlayer, bool bValue)
		{
			GET_PLAYER(ePlayer).SetDisableAutomaticFaithPurchase(bValue);
		}
		void DoApplyVassalTax(TeamTypes eMaster, PlayerTypes eVassal, int iPercent)
		{
			CvTeam& kMasterTeam = GET_TEAM(eMaster);

			if (!kMasterTeam.CanSetVassalTax(eVassal))
				return;

			iPercent = max(iPercent, /*0*/ GD_INT_GET(VASSALAGE_VASSAL_TAX_PERCENT_MINIMUM));
			iPercent = min(iPercent, /*25*/ GD_INT_GET(VASSALAGE_VASSAL_TAX_PERCENT_MAXIMUM));

			int iCurrentTaxRate = kMasterTeam.GetVassalTax(eVassal);

			kMasterTeam.SetNumTurnsSinceVassalTaxSet(eVassal, 0);
			kMasterTeam.SetVassalTax(eVassal, iPercent);

			// Note: using EspionageScreen dirty for this.
			GC.GetEngineUserInterface()->setDirty(EspionageScreen_DIRTY_BIT, true);

			// notify diplo AI if there was some change
			if(iPercent != iCurrentTaxRate)
			{
				GET_PLAYER(eVassal).GetDiplomacyAI()->DoVassalTaxChanged(eMaster, (iPercent < iCurrentTaxRate));

				// send a notification if there was some change
				Localization::String locString;
				Localization::String summaryString;
				if(iPercent > iCurrentTaxRate)
				{
					locString = Localization::Lookup("TXT_KEY_MISC_VASSAL_TAX_INCREASED");
					summaryString = Localization::Lookup("TXT_KEY_MISC_VASSAL_TAX_INCREASED_SUMMARY");
				}
				else
				{
					locString = Localization::Lookup("TXT_KEY_MISC_VASSAL_TAX_DECREASED");
					summaryString = Localization::Lookup("TXT_KEY_MISC_VASSAL_TAX_DECREASED_SUMMARY");
				}

				locString << kMasterTeam.getName().GetCString() << iCurrentTaxRate << iPercent;

				GET_PLAYER(eVassal).GetNotifications()->Add(NOTIFICATION_GENERIC, locString.toUTF8(), summaryString.toUTF8(), -1, -1, kMasterTeam.getLeaderID());
			}
		}
		void DoLiberateVassal(TeamTypes eMaster, TeamTypes eVassal)
		{
			CvTeam& kMasterTeam = GET_TEAM(eMaster);

			if (!kMasterTeam.CanLiberateVassal(eVassal))
				return;

			// End our vassalage peacefully
			GET_TEAM(eVassal).DoEndVassal(eMaster, true, false);

			// Find our vassals
			for (int iVassalPlayer = 0; iVassalPlayer < MAX_MAJOR_CIVS; iVassalPlayer++)
			{
				PlayerTypes eVassalPlayer = (PlayerTypes) iVassalPlayer;
				if (GET_PLAYER(eVassalPlayer).isAlive() && GET_PLAYER(eVassalPlayer).getTeam() == eVassal)
				{
					GET_PLAYER(eVassalPlayer).GetDiplomacyAI()->DoLiberatedFromVassalage(eMaster, false);
				}
			}
		}
		void DoClearDealRenewalID(PlayerTypes ePlayer1, PlayerTypes ePlayer2)
		{
			GC.getGame().GetGameDeals().SetRenewDealID(ePlayer1, ePlayer2, -1);
		}
	}
}