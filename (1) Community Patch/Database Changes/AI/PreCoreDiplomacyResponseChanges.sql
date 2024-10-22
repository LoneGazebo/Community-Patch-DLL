-- Remove fully duplicate rows
DELETE FROM Diplomacy_Responses
WHERE ROWID NOT IN (
	SELECT MIN(ROWID)
	FROM Diplomacy_Responses
	GROUP BY LeaderType, ResponseType, Response
);

-- Remove duplicate responses that are already used in other cases
DELETE FROM Diplomacy_Responses WHERE LeaderType = 'GENERIC' AND ResponseType = 'RESPONSE_ATTACKED_STRONG_EXCITED' AND Response = 'TXT_KEY_GENERIC_ATTACKED_EXCITED%';
DELETE FROM Diplomacy_Responses WHERE LeaderType = 'GENERIC' AND ResponseType = 'RESPONSE_ATTACKED_STRONG_HOSTILE' AND Response = 'TXT_KEY_GENERIC_ATTACKED_HOSTILE%';
DELETE FROM Diplomacy_Responses WHERE LeaderType = 'GENERIC' AND ResponseType = 'RESPONSE_ATTACKED_WEAK_EXCITED' AND Response = 'TXT_KEY_GENERIC_ATTACKED_EXCITED%';
DELETE FROM Diplomacy_Responses WHERE LeaderType = 'GENERIC' AND ResponseType = 'RESPONSE_ATTACKED_WEAK_HOSTILE' AND Response = 'TXT_KEY_GENERIC_ATTACKED_HOSTILE%';
DELETE FROM Diplomacy_Responses WHERE LeaderType = 'GENERIC' AND ResponseType = 'RESPONSE_WINNER_PEACE_OFFER' AND Response = 'TXT_KEY_GENERIC_PEACE_OFFER%';

-- Remove non-existent response type
DELETE FROM Diplomacy_Responses WHERE ResponseType = 'RESPONSE_SUSPECT_YOU_OF_SPYING';
