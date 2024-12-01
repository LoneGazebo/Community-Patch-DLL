-- First meeting
UPDATE Language_en_US
SET Text = 'Greetings, Stranger. I am Pachacuti, ruler of the peerless Incans. If there is any way that we can assist your inferior civilization, please do not hesitate to ask.'
WHERE Tag = 'TXT_KEY_LEADER_PACHACUTI_FIRSTGREETING_3';

-- Greeting (friendly)
UPDATE Language_en_US
SET Text = 'Hello, friend. What brings you to my court this day?'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_POLITE_HELLO_1';

UPDATE Language_en_US
SET Text = 'Hello again, my friend.'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_POLITE_HELLO_3';

UPDATE Language_en_US
SET Text = 'Ah, hello again, friend! What can this mighty general do for you?'
WHERE Tag = 'TXT_KEY_LEADER_ALEXANDER_GREETING_POLITE_HELLO_1';

UPDATE Language_en_US
SET Text = 'God smiles upon you, my friend. What can I do for you?'
WHERE Tag = 'TXT_KEY_LEADER_HARUN_GREETING_POLITE_HELLO_1';

-- Greeting (neutral)
UPDATE Language_en_US
SET Text = 'I trust that God willed your visit to Songhai, great leader.'
WHERE Tag = 'TXT_KEY_LEADER_ASKIA_GREETING_NEUTRAL_HELLO_1';

UPDATE Language_en_US
SET Text = 'Germany always looks forward to favorable relations with mighty empires such as yours.'
WHERE Tag = 'TXT_KEY_LEADER_BISMARCK_GREETING_NEUTRAL_HELLO_1';

UPDATE Language_en_US
SET Text = 'What brings you this far into the wilderness, leader?'
WHERE Tag = 'TXT_KEY_LEADER_HIAWATHA_GREETING_NEUTRAL_HELLO_1';

UPDATE Language_en_US
SET Text = 'Here to admire the French Empire, or perhaps to do business?'
WHERE Tag = 'TXT_KEY_LEADER_NAPOLEON_GREETING_NEUTRAL_HELLO_1';

-- Greeting (hostile)
UPDATE Language_en_US
SET Text = 'You come to mighty Persia as you are? What do you want?'
WHERE Tag = 'TXT_KEY_LEADER_DARIUS_GREETING_HOSTILE_HELLO_1';

UPDATE Language_en_US
SET Text = 'I make it a habit to speak only to great leaders, but I shall make an exception for you. Hurry up now.'
WHERE Tag = 'TXT_KEY_LEADER_SULEIMAN_GREETING_HOSTILE_HELLO_1';

-- Greeting (repeat 3-4 times, non-hostile)
UPDATE Language_en_US
SET Text = 'You sure do like talking to me, don''t you?'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_REPEAT_2';

-- Greeting (aggressive expansion, hostile)
UPDATE Language_en_US
SET Text = 'The spread of your cities and people is like a plague infecting the land. I look forward to the day I don''t have to see it anymore.'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_HOSTILE_AGGRESSIVE_EXPANSION_1';

-- Greeting (aggressive plot buying, non-hostile)
UPDATE Language_en_US
SET Text = 'Your recent trend of buying land near us is concerning. For the sake of our future relationship, I advise that you be cautious in this matter. Now that that''s out of the way, what brings you here today?'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_AGGRESSIVE_PLOT_BUYING_1';

-- Greeting (weak army, hostile)
UPDATE Language_en_US
SET Text = 'Just between you and me, your military is kind of the laughing stock of the world. Don''t let that get you down though; I''m sure someone will put you out of your misery soon.'
WHERE Tag = 'TXT_KEY_GENERIC_GREETING_HOSTILE_HUMAN_SMALL_ARMY';

-- Discuss menu
UPDATE Language_en_US
SET Text = 'Speak: anything to drown out the whispers.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_LETSHEARIT_1';

-- We can't match human's offered goods (non-hostile)
UPDATE Language_en_US
SET Text = 'We cannot offer you enough to make this a fair trade. We can, however, offer you this deal.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_CANT_MATCH_OFFER_1';

UPDATE Language_en_US
SET Text = 'The deal we suggest may be an unfair exchange for you. Please inspect it closely.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_CANT_MATCH_OFFER_2';

-- Make offer
UPDATE Language_en_US
SET Text = 'This offer will not be open long; think about it.'
WHERE Tag = 'TXT_KEY_LEADER_ENRICO_DANDOLO_TRADEREQUEST_NEUTRAL';

-- Make offer for embassy
UPDATE Language_en_US
SET Text = 'Does this trade interest you?'
WHERE Tag = 'TXT_KEY_LEADER_GENGHIS_TRADEREQUEST_HAPPY';

UPDATE Language_en_US
SET Text = 'It appears that you do have a reason for existing: to make this deal with me.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADEREQUEST_HAPPY';

-- Make offer for luxury resource
UPDATE Language_en_US
SET Text = 'It is God''s will that we seek out cooperation and trade with our fellow men when possible.'
WHERE Tag = 'TXT_KEY_LEADER_HARUN_LUXURY_TRADE_1';

UPDATE Language_en_US
SET Text = 'Ah, hello, my friend. I noticed that we could make a mutually beneficial deal. What do you think?'
WHERE Tag = 'TXT_KEY_LEADER_GANDHI_LUXURY_TRADE_1';

UPDATE Language_en_US
SET Text = 'Trade is the fruit of a relationship between two great kingdoms. What do you think of this offer, my friend?'
WHERE Tag = 'TXT_KEY_LEADER_RAMKHAMHAENG_LUXURY_TRADE_1';

-- Make offer for open borders exchange
UPDATE Language_en_US
SET Text = 'Opening our borders seems like a convenient agreement at the present time. Do you agree?'
WHERE Tag = 'TXT_KEY_LEADER_AUGUSTUS_OPEN_BORDERS_EXCHANGE_1';

UPDATE Language_en_US
SET Text = 'Opening our borders will greatly expedite the movement of troops. Should it be necessary, of course. What do you think?'
WHERE Tag = 'TXT_KEY_LEADER_NAPOLEON_OPEN_BORDERS_EXCHANGE_1';

-- Make offer for research agreement
UPDATE Language_en_US
SET Text = 'I feel that a Research Agreement is a mutually beneficial arrangement that would help serve to cement our friendship.'
WHERE Tag = 'TXT_KEY_GENERIC_RESEARCH_AGREEMENT_OFFER_1';

-- Accept generous deal
UPDATE Language_en_US
SET Text = 'Very well. Not that it will help either of us in the long run... we will all die soon enough.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADE_YES_HAPPY';

UPDATE Language_en_US
SET Text = 'Your offer pleases me. Well done.'
WHERE Tag = 'TXT_KEY_LEADER_RAMESSES_TRADE_YES_HAPPY';

UPDATE Language_en_US
SET Text = 'Happily agreed.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_TRADE_YES_HAPPY';

-- Accept fair deal
UPDATE Language_en_US
SET Text = 'Your offer is agreeable to us. We accept.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_YES_NEUTRAL';

-- Reject deal (hostile)
UPDATE Language_en_US
SET Text = 'This is not at all acceptable. If you wish for us to get anywhere, you must offer a fair deal.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_REJECT_INSULTING_1';

UPDATE Language_en_US
SET Text = 'My husband is deceased, you know: Russia is no longer ruled by an idiot. We decline.'
WHERE Tag = 'TXT_KEY_LEADER_CATHERINE_TRADE_NO_ANGRY';

UPDATE Language_en_US
SET Text = 'I have conceived of a blind, almost overpowering dislike for you. The answer is "no."'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADE_NO_ANGRY';

-- Reject deal (non-hostile)
UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ALEXANDER_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ASKIA_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_AUGUSTUS_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_BISMARCK_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_CATHERINE_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_DARIUS_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_GANDHI_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_HARUN_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_HIAWATHA_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_MONTEZUMA_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_NAPOLEON_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_ODA_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_RAMKHAMHAENG_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_SULEIMAN_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'I do not think that the trade, as it is currently configured, would be in the United States'' best interests. Would you care to improve your offer?'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_TRADE_NEEDMORE_NEUTRAL_1';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_TRADE_NEEDMORE_NEUTRAL_4';

UPDATE Language_en_US
SET Text = 'Almost...'
WHERE Tag = 'TXT_KEY_LEADER_WUZETIAN_TRADE_NEEDMORE_NEUTRAL_4';

-- Make demand
UPDATE Language_en_US
SET Text = 'You have something I want. If you don''t hand it over quietly, I''ll have no choice but to take it by force.'
WHERE Tag = 'TXT_KEY_GENERIC_DEMAND_1';

UPDATE Language_en_US
SET Text = 'Give me what I want, and I may spare you... for now.'
WHERE Tag = 'TXT_KEY_LEADER_GAJAH_MADA_DEMANDTRIBUTE_NEUTRAL';

UPDATE Language_en_US
SET Text = 'The following tribute would improve my black humor, greatly increasing the odds that you would survive another day.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_DEMANDTRIBUTE_NEUTRAL';

-- Human accepts demand
UPDATE Language_en_US
SET Text = 'Oh, very well; I suppose I must. We are agreed.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_YES_ANGRY';

UPDATE Language_en_US
SET Text = 'Smart move.'
WHERE Tag = 'TXT_KEY_LEADER_GAJAH_MADA_TRADE_YES_ANGRY';

-- Human rejects demand or gives mean response (not declaring war)
UPDATE Language_en_US
SET Text = 'Very well. It appears as if I have no choice. But someday, you will pay for this.'
WHERE Tag = 'TXT_KEY_LEADER_ALEXANDER_TRIBUTE_YES_ANGRY';

UPDATE Language_en_US
SET Text = 'You have us at a disadvantage now, but it may not always be so. We shall remember this.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRIBUTE_YES_ANGRY';

UPDATE Language_en_US
SET Text = 'I will agree to your reprehensible demands.'
WHERE Tag = 'TXT_KEY_LEADER_MARIA_I_TRIBUTE_YES_ANGRY';

UPDATE Language_en_US
SET Text = 'Your fetid breath chokes the life from me! Take what you would and be gone.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRIBUTE_YES_ANGRY';

UPDATE Language_en_US
SET Text = 'Very well. I hope you may choke on it!'
WHERE Tag = 'TXT_KEY_LEADER_PACHACUTI_TRIBUTE_YES_ANGRY';

-- Accept demand
UPDATE Language_en_US
SET Text = 'You may have this bit of material wealth, yes. But such indiscretions are not easily forgotten.'
WHERE Tag = 'TXT_KEY_LEADER_BOUDICCA_TRIBUTE_YES_NEUTRAL';

UPDATE Language_en_US
SET Text = 'Oh, very well; I suppose we have no choice.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRIBUTE_YES_NEUTRAL';

UPDATE Language_en_US
SET Text = 'I agree; if only to confound my advisors, who urge me to refuse.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRIBUTE_YES_NEUTRAL';

UPDATE Language_en_US
SET Text = 'It honors my people to help those in need.'
WHERE Tag = 'TXT_KEY_LEADER_POCATELLO_TRIBUTE_YES_NEUTRAL';

UPDATE Language_en_US
SET Text = 'Very well. It appears that I have no choice but to accede to your demand. But I warn you not to press us too hard.'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_TRIBUTE_YES_NEUTRAL';

-- Reject impossible demand
UPDATE Language_en_US
SET Text = 'We implore you not to waste our time with such foolishness.'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRADE_NO_NEUTRAL';

UPDATE Language_en_US
SET Text = 'You must be insane to insult me with such an offer. We refuse.'
WHERE Tag = 'TXT_KEY_LEADER_GENGHIS_TRADE_NO_NEUTRAL';

UPDATE Language_en_US
SET Text = 'Your offer is declined. I would be insulted; that is, if I cared anything about you.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRADE_NO_NEUTRAL';

-- Reject demand (human too far, too weak, or can't declare war; or our master will protect us)
-- Can also be triggered by Player:DoForceDenounce() in Lua, for some reason
UPDATE Language_en_US
SET Text = 'Foolish Jackal! You would do well to go to Egypt, where such as you are worshipped. Here, you get nothing.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_TRIBUTE_NO_NEUTRAL';

-- Reject demand (human untrustworthy, we're planning war with them, or our boldness is 10)
UPDATE Language_en_US
SET Text = 'You dare insult me in such a manner? If you want something so badly, then do your best to come and take it.'
WHERE Tag = 'TXT_KEY_GENERIC_HUMAN_DEMAND_REFUSE_HOSTILE_1';

UPDATE Language_en_US
SET Text = 'If you want that, you''ll have to come and get it.'
WHERE Tag = 'TXT_KEY_GENERIC_HUMAN_DEMAND_REFUSE_HOSTILE_3';

UPDATE Language_en_US
SET Text = 'You presume to demand tribute from us? Go away, you hateful villain!'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_TRIBUTE_NO_ANGRY';

-- Make request
UPDATE Language_en_US
SET Text = 'Unfortunately, things aren''t going terribly well over here. Can you spare something for a friend? I''ll try to return the favor when things are going better for me.'
WHERE Tag = 'TXT_KEY_GENERIC_REQUEST_ITEM_1';

UPDATE Language_en_US
SET Text = 'Circumstances have not been kind to the Songhai lately, friend. I believe God wishes to teach us that we cannot triumph alone.'
WHERE Tag = 'TXT_KEY_LEADER_ASKIA_RESPONSE_REQUEST_1';

UPDATE Language_en_US
SET Text = 'The United States would be indebted to you if you were to provide us assistance.'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_RESPONSE_REQUEST_1';

-- Taunt human after attacking a city state under their protection (non-hostile)
UPDATE Language_en_US
SET Text = 'It''s come to my attention that I may have attacked a City-State with which you have had past relations. While it was not my goal to be at odds with your empire, this was deemed a necessary course of action.'
WHERE Tag = 'TXT_KEY_WE_ATTACKED_YOUR_MINOR_1';

UPDATE Language_en_US
SET Text = 'I''ve been informed that my empire has commenced an attack on a City-State friendly to you. I assure you, this was not on purpose, and I hope that this does not serve to drive us apart.'
WHERE Tag = 'TXT_KEY_WE_ATTACKED_YOUR_MINOR_2';

-- Too many military units near our borders (hostile)
UPDATE Language_en_US
SET Text = 'I see your armies amassed near my empire. If you wish to attack, then come. Don''t hide in the shadows like a child.'
WHERE Tag = 'TXT_KEY_HOSTILE_AGGRESSIVE_MILITARY_WARNING_1';

UPDATE Language_en_US
SET Text = 'Should your massed armies cross my border, they shall have the wrath of all of China unleashed upon them.'
WHERE Tag = 'TXT_KEY_WUZETIAN_HOSTILE_AGGRESSIVE_MILITARY_WARNING_1';

UPDATE Language_en_US
SET Text = 'Ha! What are your forces doing near my territory? If you wish to fight, know that you cannot beat me.'
WHERE Tag = 'TXT_KEY_NAPOLEON_HOSTILE_AGGRESSIVE_MILITARY_WARNING_1';

-- Human attacked a city state under our protection and haven't made a promise before
UPDATE Language_en_US
SET Text = 'Your wanton aggression against {@1_MinorCivName} does not go unnoticed. You will face serious consequences should you continue your assault.'
WHERE Tag = 'TXT_KEY_ATTACKED_PROTECTED_CITY_STATE_1';

-- Human denounces us
UPDATE Language_en_US
SET Text = 'So, is that how it is, then? Very well. I shall not forget this.'
WHERE Tag = 'TXT_KEY_RESPONSE_TO_BEING_DENOUNCED_1';

-- Denounce human friend
UPDATE Language_en_US
SET Text = 'I''ve had enough of you. I''ve told the other leaders of the world that working with you is a mistake.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DENOUNCING_FRIEND_1';

UPDATE Language_en_US
SET Text = 'Alright, enough is enough. Informing everyone else of your ill deeds is long overdue.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DENOUNCING_FRIEND_2';

-- Denounce human (no specific reason)
UPDATE Language_en_US
SET Text = 'It''s time I told the world of your sins.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DENOUNCE_1';

UPDATE Language_en_US
SET Text = 'I''ve spoken to the other leaders about you. They need to know you are not to be trusted.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DENOUNCE_2';

UPDATE Language_en_US
SET Text = 'Just so you''re aware, the other leaders have now heard the grim truth about you.[NEWLINE][NEWLINE]([COLOR_WARNING_TEXT]They have publicly denounced us![ENDCOLOR])'
WHERE Tag = 'TXT_KEY_DENOUNCE_3';

-- Human declares war on us (default case, similar military strength)
UPDATE Language_en_US
SET Text = 'Why, you... you... worthless barren mule! I will crush you!'
WHERE Tag = 'TXT_KEY_LEADER_DARIUS_ATTACKED_1';

UPDATE Language_en_US
SET Text = 'Unfortunately, not everybody in my country is as committed to non-violence as I am, so please let me apologize in advance if your forces are destroyed to the last man.'
WHERE Tag = 'TXT_KEY_LEADER_GANDHI_ATTACKED_2';

UPDATE Language_en_US
SET Text = 'We shall destroy you, you know. Do you care for some cheese?'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_ATTACKED_2';

UPDATE Language_en_US
SET Text = 'Oh well. I presume you know what you''re doing.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_ATTACKED_3';

UPDATE Language_en_US
SET Text = 'What cursed treachery is this? Have you no honor, no shame?'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_ATTACKED_1';

UPDATE Language_en_US
SET Text = 'You sad little person. Soon you shall feel the full weight of our mighty science bearing down upon you.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_ATTACKED_2';

UPDATE Language_en_US
SET Text = 'Excellent, excellent! I have been looking for a reason to try out my newly outfitted army, and you have provided me with just the occasion.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_ATTACKED_3';

-- Human declares war on us after promised not to
UPDATE Language_en_US
SET Text = 'Foolish, foolish...'
WHERE Tag = 'TXT_KEY_LEADER_HIAWATHA_WARBLUFF_1';

UPDATE Language_en_US
SET Text = 'The drums - the drums! They pound in my brain, drowning out thoughts of vengeance.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_WARBLUFF_1';

UPDATE Language_en_US
SET Text = 'The next time, I shall not be so polite.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_WARBLUFF_4';

UPDATE Language_en_US
SET Text = 'Very well. The next time, I shall not be so polite!'
WHERE Tag = 'TXT_KEY_LEADER_WASHINGTON_WARBLUFF_2';

-- Declare war (they're stronger than us)
UPDATE Language_en_US
SET Text = 'Destruction might be the fate which awaits me, but should I back down, only eventual defeat awaits anyway.'
WHERE Tag = 'TXT_KEY_GENERIC_DOW_DESPERATE_8';

-- Declare war
-- 1. We taunted human after attacking their protected city state and they selected the mean response
-- 2. We taunted human after bullying their protected city state and they selected the mean response
-- 3. Active war declaration, but not hitting any of the other messages
UPDATE Language_en_US
SET Text = 'I had a feeling that the two of us would someday meet in the field of battle. We will now see which of us is meant to be victorious.'
WHERE Tag = 'TXT_KEY_GENERIC_DOW_GENERIC_2';

UPDATE Language_en_US
SET Text = 'It is time to fight. May the best of us win.'
WHERE Tag = 'TXT_KEY_GENERIC_DOW_GENERIC_3';

UPDATE Language_en_US
SET Text = 'Your many insults to us and to our loyal subjects will not go unpunished - prepare for war!'
WHERE Tag = 'TXT_KEY_LEADER_ELIZABETH_DECLAREWAR_1';

UPDATE Language_en_US
SET Text = 'I am sending a contingent of Indian soldiers on a goodwill tour of your major cities. I trust that they will be received with open arms.'
WHERE Tag = 'TXT_KEY_LEADER_GANDHI_DECLAREWAR_2';

UPDATE Language_en_US
SET Text = 'It was inevitable that we would come to blows, and now is as good a time as another.'
WHERE Tag = 'TXT_KEY_LEADER_GENGHIS_DECLAREWAR_1';

UPDATE Language_en_US
SET Text = 'Your people remain backwards and primitive. Clearly a change of regime is in order. Prepare for war!'
WHERE Tag = 'TXT_KEY_LEADER_PACHACUTI_DECLAREWAR_1';

UPDATE Language_en_US
SET Text = 'The Hall grows weary of your tiresome antics. We shall forcefully liberate your downtrodden, and lead them to enlightenment.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_DECLAREWAR_1';

UPDATE Language_en_US
SET Text = 'Time waits for no man, and I feel its changeless march growing ever more urgent. You stand in our way, and I have not the resources left for more diplomatic means.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_DECLAREWAR_2';

UPDATE Language_en_US
SET Text = 'I''ve experiments to run, research to be done, and here you stand like an aging wall to block my path. I shall remove you.'
WHERE Tag = 'TXT_KEY_LEADER_SEJONG_DECLAREWAR_3';

-- Declare war (human rejected our demand)
UPDATE Language_en_US
SET Text = 'So be it. My armies will extract it from your lands, then.'
WHERE Tag = 'TXT_KEY_WAR_DEMAND_REFUSED_1';

-- Human refuses to make promise
-- 1. Attacked our protected city states
-- 2. Bullied our protected city states
-- 3. Aggressive expansion
-- 4. Aggressive plot buying
UPDATE Language_en_US
SET Text = 'I have no more business with the likes of you, then.'
WHERE Tag = 'TXT_KEY_GENERIC_HUMAN_RESPONSE_BAD_2';

-- Insult human (they're an easy target)
UPDATE Language_en_US
SET Text = 'With an army as weak as yours, it''s a surprise your empire hasn''t succumb to a barbarian invasion.'
WHERE Tag = 'TXT_KEY_GENERIC_INSULT_MILITARY_2';

-- Insult human (they're very unhappy)
UPDATE Language_en_US
SET Text = 'I can hear your people wailing in sorrow all the way over in my empire. If you don''t do something soon, they might just pack up and find a real civilization to live in.'
WHERE Tag = 'TXT_KEY_GENERIC_INSULT_UNHAPPINESS_1';

UPDATE Language_en_US
SET Text = 'With your people so unhappy, it''s a shock to me that they''d keep you around as their leader.'
WHERE Tag = 'TXT_KEY_GENERIC_INSULT_UNHAPPINESS_2';

-- Insult human (we have double their population)
UPDATE Language_en_US
SET Text = 'Travelers have told us that your empire''s economy is in pretty sad shape. If you beg a little, I might give you a hand. Okay, actually, I won''t.'
WHERE Tag = 'TXT_KEY_GENERIC_INSULT_POPULATION_1';

-- Insult human (not hitting any of the other messages)
UPDATE Language_en_US
SET Text = 'I couldn''t help but notice how pathetic you are. And when I realized it, I couldn''t help but share.'
WHERE Tag = 'TXT_KEY_GENERIC_INSULT_GENERIC_1';

-- Offer peace (warscore >= 10)
UPDATE Language_en_US
SET Text = 'My sword is true, and mighty as any hero''s. I hope you''ve learned your lesson, and to Carthage come no more.'
WHERE Tag = 'TXT_KEY_LEADER_DIDO_WINWAR_1';

UPDATE Language_en_US
SET Text = 'Oh, what is that sound I hear? Wait... I believe it is the crying of your women! Ha! Another victory for me!'
WHERE Tag = 'TXT_KEY_LEADER_HARALD_WINWAR_1';

UPDATE Language_en_US
SET Text = 'A fine day and a fine battle, wouldn''t you agree? We shall have to do this again sometime.'
WHERE Tag = 'TXT_KEY_LEADER_HARALD_WINWAR_2';

UPDATE Language_en_US
SET Text = 'I hope you have learned your lesson. I am invincible!'
WHERE Tag = 'TXT_KEY_LEADER_NAPOLEON_WINWAR_1';

UPDATE Language_en_US
SET Text = 'That was refreshing. If the demons are sated, perhaps they will give us a little respite now.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_WINWAR_1';

-- Human eliminates us
UPDATE Language_en_US
SET Text = 'You have done well. I shall put in a good word for you to the demons.'
WHERE Tag = 'TXT_KEY_LEADER_NEBUCHADNEZZAR_DEFEATED_1';

UPDATE Language_en_US
SET Text = 'This is why I prefer diplomacy to violence!'
WHERE Tag = 'TXT_KEY_LEADER_PEDRO_DEFEATED_1';

UPDATE Language_en_US
SET Text = 'My kingdom will always remember its true king... regardless of your petty victory.'
WHERE Tag = 'TXT_KEY_LEADER_CASIMIR_DEFEATED_3';

-- Human ends our friendship
UPDATE Language_en_US
SET Text = 'Your actions are unforgivable. There can never be reconciliation between our two nations.[NEWLINE][NEWLINE](If you denounce this player or declare war on them within the next [COLOR_WARNING_TEXT]10[ENDCOLOR] turns, you will receive a diplomatic penalty for backstabbing them.)'
WHERE Tag = 'TXT_KEY_NOW_UNFORGIVABLE_1';

-- Break friendship with human (high warmonger threat)
UPDATE Language_en_US
SET Text = 'I did not want things to end up like this, but I cannot overlook your warmongering. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_WARMONGER_1';

-- Break friendship with human (competing for city states)
UPDATE Language_en_US
SET Text = 'Your dealings with City-States in my sphere of influence are unacceptable. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_MINORS_1';

-- Break friendship with human (territory disputes)
UPDATE Language_en_US
SET Text = 'I fear that our close proximity was destined to poison our relationship. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_LAND_1';

-- Break friendship with human (competing for world wonders)
UPDATE Language_en_US
SET Text = 'Your continued greed in collecting wonders of the world has become a major issue. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_WONDERS_1';

-- Break friendship with human (competing for same victory condition)
UPDATE Language_en_US
SET Text = 'It is clear to me now that the paths we are traveling must ultimately drive us apart. Consider our Declaration of Friendship ended.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_VICTORY_1';

-- Break friendship with human (hitting none of the other messages)
UPDATE Language_en_US
SET Text = 'Circumstances have changed, and I believe there is no longer a need for our Declaration of Friendship to continue. I hope you can understand.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_DEFAULT_1';

UPDATE Language_en_US
SET Text = 'I am sorry, but it is time for our Declaration of Friendship to end.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_DEFAULT_2';
