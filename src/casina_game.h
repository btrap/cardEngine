#pragma once

#include "variable_defines.h"

#include "memory.h"

enum GAME_PLAYERS
{
    GAME_PLAYER_NONE = 0,
    GAME_PLAYER_ONE,
    GAME_PLAYER_TWO,
    GAME_PLAYER_THREE,
    GAME_PLAYER_FOUR,
};

enum STANDARD_CARD_SUITS
{
    STANDARD_CARD_BLANK = 0,
    STANDARD_CARD_SPADE,
    STANDARD_CARD_HEART,
    STANDARD_CARD_DIAMOND,
    STANDARD_CARD_CLUB,
};

smax GetBoardStep(smax index)
{
    switch(index)
    {
        case 0:
        case 1:
            return 2;
        case 2:
        case 3:
            return 3;
        case 4:
        case 5:
            return 1;
        case 6:
        case 7:
            return 4;
        case 8:
        case 9:
            return 0;
        case 10:
        case 11:
            return 5;
    }
    return 0;
}

const char * GetSuitString(STANDARD_CARD_SUITS suit)
{
    switch(suit)
    {
        case STANDARD_CARD_SPADE:
        {
            return "SPADE";
        }break;
        case STANDARD_CARD_HEART:
        {
            return "HEART";
        }break;
        case STANDARD_CARD_DIAMOND:
        {
            return "DIAMOND";
        }break;
        case STANDARD_CARD_CLUB:
        {
            return "CLUB";
        }break;
        case STANDARD_CARD_BLANK:
        default:
        {
            return "BLANK";
        }break;
    }

    return "INVALID SUIT";
}

enum STANDARD_CARD_VALUES
{
    STANDARD_CARD_JOKER = 0,
    STANDARD_CARD_ACE,
    STANDARD_CARD_2,
    STANDARD_CARD_3,
    STANDARD_CARD_4,
    STANDARD_CARD_5,
    STANDARD_CARD_6,
    STANDARD_CARD_7,
    STANDARD_CARD_8,
    STANDARD_CARD_9,
    STANDARD_CARD_10,
    STANDARD_CARD_JACK,
    STANDARD_CARD_QUEEN,
    STANDARD_CARD_KING,
};

const char * GetValueString(STANDARD_CARD_VALUES value)
{
    switch(value)
    {
        case STANDARD_CARD_ACE:
        {
            return "A";
        }break;
        case STANDARD_CARD_2:
        {
            return "2";
        }break;
        case STANDARD_CARD_3:
        {
            return "3";
        }break;
        case STANDARD_CARD_4:
        {
            return "4";
        }break;
        case STANDARD_CARD_5:
        {
            return "5";
        }break;
        case STANDARD_CARD_6:
        {
            return "6";
        }break;
        case STANDARD_CARD_7:
        {
            return "7";
        }break;
        case STANDARD_CARD_8:
        {
            return "8";
        }break;
        case STANDARD_CARD_9:
        {
            return "9";
        }break;
        case STANDARD_CARD_10:
        {
            return "10";
        }break;
        case STANDARD_CARD_JACK:
        {
            return "J";
        }break;
        case STANDARD_CARD_QUEEN:
        {
            return "Q";
        }break;
        case STANDARD_CARD_KING:
        {
            return "K";
        }break;
        case STANDARD_CARD_JOKER:
        default:
        {
            return "JOKER";
        }break;
    }

    return "INVALID VALUE";
}

enum STANDARD_CARD_DECKS
{
    STANDARD_DECK_SUIT_COUNT = STANDARD_CARD_CLUB,
    STANDARD_DECK_VALUE_COUNT = STANDARD_CARD_KING,
    STANDARD_DECK_SIZE = STANDARD_DECK_SUIT_COUNT * STANDARD_DECK_VALUE_COUNT,
};

enum CASINA_POSITIONS
{
    CASINA_DECK,
    CASINA_HAND,
    CASINA_STACK,
    CASINA_BOARD
};

enum CASINA_CARD
{
    CASINA_DRAW_COUNT = 2,
    CASINA_HAND_SIZE = 4,
    CASINA_LITTLE_CASINA_SCORE = 1,
    CASINA_BIG_CASINA_SCORE = 2,
    CASINA_ACE_SCORE = 1,
    CASINA_MOST_SPADES_SCORE = 1,
    CASINA_MOST_CARDS_SCORE = 3,
    CASINA_WIN_SCORE = 21,
};

struct StandardCard
{
    STANDARD_CARD_SUITS suit;
    STANDARD_CARD_VALUES value;
};

struct StandardCardSet
{
    StandardCard **cards;
    umax card_Count;
};

struct StandardCardData
{
    StandardCard *cards;
    umax card_Count;
};

struct CasinaCombo
{
    StandardCardSet combo; // max 6 * 3 cards
    umax value;
    bool stacked;
};

struct CasinaPlayer
{
    StandardCardSet hand; // max 4
    StandardCardSet score_Pile; // max 52
    umax score;
};

struct CasinaBoard
{
    GAME_PLAYERS current_Player;
    GAME_PLAYERS last_To_Score;

    StandardCardData playset; // all cards needed for a full game

    StandardCardSet deck; // max 52

    CasinaCombo *combos; // max 10
    umax combo_Count;

    CasinaPlayer player1;
    CasinaPlayer player2;
};

inline void CreateCasinaPlayset(StandardCardData *playset)
{
    playset->card_Count = STANDARD_DECK_SIZE;
    STORE_PERM_DATA(playset->cards, playset->card_Count);
    for(int i = 0; i < playset->card_Count; ++i)
    {
        playset->cards[i].suit = (STANDARD_CARD_SUITS)(STANDARD_CARD_SPADE + i / STANDARD_CARD_KING);
        playset->cards[i].value = (STANDARD_CARD_VALUES)(STANDARD_CARD_ACE + i % STANDARD_CARD_KING);
    }
}

umax ScoreCasinaCards(StandardCard **cards, umax cards_Count)
{
    umax cards_Score = 0; 
    for(umax i = 0; i < cards_Count; ++i)
    {
        if((cards[i]->suit == STANDARD_CARD_DIAMOND) && (cards[i]->value == STANDARD_CARD_10))
        {
             cards_Score += CASINA_BIG_CASINA_SCORE;
        }

        if((cards[i]->suit == STANDARD_CARD_SPADE) && (cards[i]->value == STANDARD_CARD_2))
        {
             cards_Score += CASINA_LITTLE_CASINA_SCORE;
        }

        if(cards[i]->value == STANDARD_CARD_ACE)
        {
             cards_Score += CASINA_ACE_SCORE;
        }
    }

    return cards_Score;
}

umax ScoreCasinaCards(StandardCardSet *card_Set)
{
    return ScoreCasinaCards(card_Set->cards, card_Set->card_Count);
}

GAME_PLAYERS MostCards(StandardCardSet *card_Set1, StandardCardSet *card_Set2)
{
    GAME_PLAYERS has_Most_Cards = GAME_PLAYER_NONE;

    if(card_Set1->card_Count > card_Set2->card_Count)
    {
        has_Most_Cards = GAME_PLAYER_ONE;
    }

    if(card_Set2->card_Count > card_Set1->card_Count)
    {
        has_Most_Cards = GAME_PLAYER_TWO;
    }
   
   return has_Most_Cards;
}

GAME_PLAYERS MostSpades(StandardCardSet *card_Set1, StandardCardSet *card_Set2)
{
    GAME_PLAYERS has_Most_Spades = GAME_PLAYER_NONE;

    umax p1_Spade_Count = 0;
    for(umax i = 0; i < card_Set1->card_Count; ++i)
    {
        if(card_Set1->cards[i]->suit == STANDARD_CARD_SPADE)
        {
            ++p1_Spade_Count;
        }
    }
    
    umax p2_Spade_Count = 0;
    for(umax i = 0; i < card_Set2->card_Count; ++i)
    {
        if(card_Set2->cards[i]->suit == STANDARD_CARD_SPADE)
        {
            ++p2_Spade_Count;
        }
    }

    if(p1_Spade_Count > p2_Spade_Count)
    {
        has_Most_Spades = GAME_PLAYER_ONE;
    }
    if(p1_Spade_Count < p2_Spade_Count)
    {
        has_Most_Spades = GAME_PLAYER_TWO;
    }
    // has_Most_Spades = spade_Count > (STANDARD_DECK_VALUE_COUNT / 2) ? GAME_PLAYER_ONE : GAME_PLAYER_TWO;
    return has_Most_Spades;
}

umax ScoreCasinaBoard(CasinaBoard *casina_Board)
{
    umax player1_Score = ScoreCasinaCards(&casina_Board->player1.score_Pile);
    umax player2_Score = ScoreCasinaCards(&casina_Board->player2.score_Pile);

    GAME_PLAYERS who_Has_Most_Cards = MostCards(&casina_Board->player1.score_Pile, &casina_Board->player2.score_Pile);
    if(who_Has_Most_Cards == GAME_PLAYER_ONE)
    {
        player1_Score += CASINA_MOST_CARDS_SCORE;
    }
    if(who_Has_Most_Cards == GAME_PLAYER_TWO)
    {
        player2_Score += CASINA_MOST_CARDS_SCORE;
    }

    GAME_PLAYERS who_Has_Most_Spades = MostSpades(&casina_Board->player1.score_Pile, &casina_Board->player2.score_Pile);
    if(who_Has_Most_Spades == GAME_PLAYER_ONE)
    {
        player1_Score += CASINA_MOST_SPADES_SCORE;
    }
    else
    {
        player2_Score += CASINA_MOST_SPADES_SCORE;
    }

    player1_Score += ScoreCasinaCards(&casina_Board->player1.score_Pile);
    player2_Score += ScoreCasinaCards(&casina_Board->player2.score_Pile);

    casina_Board->player1.score = player1_Score;
    casina_Board->player2.score = player2_Score;

    return player1_Score + player2_Score;
}

// TODO: Remove GetRandomValue
inline umax GetRandom(umax start, umax end)
{
    umax random_Value = (umax)GetRandomValue((int)start, (int)end);
    return random_Value;
}

void SwitchCards(StandardCard **card_A, StandardCard **card_B)
{
    StandardCard *temp_Card = *card_A;
    *card_A = *card_B;
    *card_B = temp_Card;
}

void ShuffleStandardCardSet(StandardCardSet *deck)
{
    umax rng_Index = 0;
    for(umax i = 0; i < deck->card_Count; ++i)
    {
        rng_Index = GetRandom(i, deck->card_Count-1);
        SwitchCards(&deck->cards[i], &deck->cards[rng_Index]);
    }
}

void SetupCasinaGame(CasinaBoard *casina_Board)
{
    casina_Board->deck.card_Count = STANDARD_DECK_SIZE;
    ASSERT(casina_Board->deck.card_Count <= casina_Board->playset.card_Count);
    STORE_PERM_DATA(casina_Board->deck.cards, casina_Board->deck.card_Count);
    for(int i = 0; i < casina_Board->deck.card_Count; ++i)
    {
        casina_Board->deck.cards[i] = &casina_Board->playset.cards[i];
    }

    casina_Board->combo_Count = 0;
    STORE_PERM_DATA(casina_Board->combos, 12);
    for(umax i = 0; i < 10; ++i)
    {
        casina_Board->combos[i].stacked = false;
        casina_Board->combos[i].value = 0;
        casina_Board->combos[i].combo.card_Count = 0;
        STORE_PERM_DATA(casina_Board->combos[i].combo.cards, 6 * 3);
    }

    casina_Board->player1.score = 0;
    casina_Board->player1.hand.card_Count = 0;
    STORE_PERM_DATA(casina_Board->player1.hand.cards, CASINA_HAND_SIZE);
    casina_Board->player1.score_Pile.card_Count = 0;
    STORE_PERM_DATA(casina_Board->player1.score_Pile.cards, STANDARD_DECK_SIZE);

    casina_Board->player2.score = 0;
    casina_Board->player2.hand.card_Count = 0;
    STORE_PERM_DATA(casina_Board->player2.hand.cards, CASINA_HAND_SIZE);
    casina_Board->player2.score_Pile.card_Count = 0;
    STORE_PERM_DATA(casina_Board->player2.score_Pile.cards, STANDARD_DECK_SIZE);
}

StandardCardSet* GetActivePlayerHand(CasinaBoard *casina_Board)
{
    StandardCardSet *player_Hand = (casina_Board->current_Player == GAME_PLAYER_ONE) ? &casina_Board->player1.hand : &casina_Board->player2.hand;
    return player_Hand;
}


StandardCard* DrawCard(StandardCardSet *set)
{
    StandardCard *card = set->cards[set->card_Count-1];
    --set->card_Count;
    return card;
}

StandardCard* DrawCard(StandardCardSet *set, umax index)
{
    ASSERT(index < set->card_Count);
    StandardCard *card = set->cards[index];
    for(umax i = index; i < set->card_Count-1; ++i)
    {
        set->cards[i] = set->cards[i+1];
    }
    --set->card_Count;
    return card;
}

void AddCard(StandardCardSet *set, StandardCard *card)
{
    set->cards[set->card_Count] = card;
    ++set->card_Count;
}

void DrawCardFromSet(StandardCardSet *deck, StandardCardSet *hand)
{
    StandardCard * card = DrawCard(deck);
    AddCard(hand, card);
}

void MoveCardFromSet(StandardCardSet *board, StandardCardSet *hand, umax index)
{
    StandardCard * card = DrawCard(hand, index);
    AddCard(board, card);
}

void RemoveCombo(CasinaBoard *casina_Board, umax combo_Index)
{
    CasinaCombo *combo_A = casina_Board->combos + combo_Index;
    CasinaCombo *combo_B = casina_Board->combos + casina_Board->combo_Count-1;

    StandardCard **cards = combo_A->combo.cards;

    combo_A->combo.cards = combo_B->combo.cards;
    combo_A->combo.card_Count = combo_B->combo.card_Count;
    combo_A->stacked = combo_B->stacked;
    combo_A->value = combo_B->value;
    
    combo_B->combo.cards = cards;
    combo_B->combo.card_Count = 0;
    combo_B->stacked = false;
    combo_B->value = 0;
    
    --casina_Board->combo_Count;
}

void DrawCardsFromSet(StandardCardSet *deck, StandardCardSet *hand, umax card_Count)
{
    while(card_Count > 0)
    {
        DrawCardFromSet(deck, hand);
        --card_Count;
    }
}

void StackCard(CasinaBoard *casina_Board, umax hand_Index, umax stack_Index)
{
    StandardCardSet *active_Player_Hand = GetActivePlayerHand(casina_Board);
    MoveCardFromSet(&casina_Board->combos[stack_Index].combo, active_Player_Hand, hand_Index);
}

void StackCard(CasinaBoard *casina_Board, umax hand_Index)
{
    StackCard(casina_Board, hand_Index, casina_Board->combo_Count);
    ++casina_Board->combo_Count;
}

void StackCard(CasinaBoard *casina_Board, StandardCard *card, umax stack_Index)
{
    StandardCardSet *active_Player_Hand = GetActivePlayerHand(casina_Board);
    umax hand_Index = 0;
    for(umax i = 0; i < active_Player_Hand->card_Count; ++i)
    {
        if(active_Player_Hand->cards[i] == card)
        {
            hand_Index = i;
        }
    }

    StackCard(casina_Board, hand_Index, stack_Index);
}

void StackCard(CasinaBoard *casina_Board, StandardCard *card)
{
    ++casina_Board->combo_Count;
    StackCard(casina_Board, card, casina_Board->combo_Count);
}

void ScoreHandCard(CasinaBoard *casina_Board, umax hand_Index)
{
    StandardCardSet *active_Player_Hand = GetActivePlayerHand(casina_Board);
    StandardCardSet *active_Score_Pile = (casina_Board->current_Player == GAME_PLAYER_ONE) ? &casina_Board->player1.score_Pile : &casina_Board->player2.score_Pile;
    MoveCardFromSet(active_Score_Pile, active_Player_Hand, hand_Index);
}

void ScoreBoardCard(CasinaBoard *casina_Board, umax combo_Index)
{
    StandardCardSet *active_Score_Pile = (casina_Board->current_Player == GAME_PLAYER_ONE) ? &casina_Board->player1.score_Pile : &casina_Board->player2.score_Pile;
    while(casina_Board->combos[combo_Index].combo.card_Count)
    {
        MoveCardFromSet(active_Score_Pile, &casina_Board->combos[combo_Index].combo, 0);
    }
    RemoveCombo(casina_Board, combo_Index);
}

void DrawOutRound(CasinaBoard *casina_Board)
{
    DrawCardsFromSet(&casina_Board->deck, &casina_Board->player1.hand, CASINA_DRAW_COUNT);
    DrawCardsFromSet(&casina_Board->deck, &casina_Board->player2.hand, CASINA_DRAW_COUNT);

    DrawCardsFromSet(&casina_Board->deck, &casina_Board->player1.hand, CASINA_DRAW_COUNT);
    DrawCardsFromSet(&casina_Board->deck, &casina_Board->player2.hand, CASINA_DRAW_COUNT);
}

void DragCardToBoard(StandardCardSet *board, StandardCardSet *player_Hand, umax index)
{
    MoveCardFromSet(board, player_Hand, index);
}

void PlayCasinaRound(CasinaBoard *casina_Board)
{
    GAME_PLAYERS current_Player = GAME_PLAYER_ONE;
    umax turn_Count = CASINA_HAND_SIZE;

    while(turn_Count > (CASINA_HAND_SIZE * 2))
    {
        current_Player = (current_Player == GAME_PLAYER_ONE) ? GAME_PLAYER_TWO : GAME_PLAYER_ONE;
        --turn_Count;
    }

    // for(while turn count * 2)
        // Drag with no collision
        // User drags a card from the board or the hand.
        // The User drags to another card of blank spot.
        // Test if the user has a card in the hand that equals that sum.
        // If it does allow it, else reset.
        // buttons for reset and finish turn.
        // If finish turn pressed Switch player and resart.
}

void SwitchCurrentPlayer(CasinaBoard *casina_Board)
{
    if(casina_Board->current_Player == GAME_PLAYER_ONE)
    {
        casina_Board->current_Player = GAME_PLAYER_TWO;
    }
    else
    {
        casina_Board->current_Player = GAME_PLAYER_ONE;
    }
}

GAME_PLAYERS GetWinner(CasinaPlayer player1, CasinaPlayer player2)
{
    GAME_PLAYERS winner = GAME_PLAYER_NONE;

    if(!(player1.score == player2.score))
    {
        if(player1.score > 21)
        {
            winner = GAME_PLAYER_ONE;
        }

        if(player2.score > 21)
        {
            winner = GAME_PLAYER_TWO;
        }
    }

    return winner;
}

bool RoundOver(CasinaBoard *casina_Board)
{
    if(casina_Board->player2.hand.card_Count == 0)
    {
        ScoreCasinaBoard(casina_Board);
        if(casina_Board->deck.card_Count < 1)
        {
            //StandardCardSet score_Pile = (casina_Board->last_To_Score == GAME_PLAYER_ONE) ? casina_Board->player1.score_Pile : casina_Board->player2->score_Pile;
            casina_Board->current_Player = casina_Board->last_To_Score;
            while(casina_Board->combo_Count > 0)
            {
                ScoreBoardCard(casina_Board, 0);
            }
        }
        else
        {
            DrawOutRound(casina_Board);
        }
        return true;
    }

    return false;
}

bool UpdatePlay(CasinaBoard *casina_Board)
{
    GAME_PLAYERS game_Winner = GAME_PLAYER_NONE;
    if(game_Winner != GAME_PLAYER_NONE)
    {
        return false;
    }

    PlayCasinaRound(casina_Board);
    ScoreCasinaBoard(casina_Board);
    game_Winner = GetWinner(casina_Board->player1, casina_Board->player2);

    return true;
}

void StartCasinaGame(CasinaBoard *casina_Board)
{
    casina_Board->current_Player = GAME_PLAYER_ONE;
    casina_Board->last_To_Score = GAME_PLAYER_NONE;

    ShuffleStandardCardSet(&casina_Board->deck);

    DrawCardsFromSet(&casina_Board->deck, &casina_Board->player1.hand, CASINA_DRAW_COUNT);
    DrawCardsFromSet(&casina_Board->deck, &casina_Board->player2.hand, CASINA_DRAW_COUNT);
    DrawCardsFromSet(&casina_Board->deck, &casina_Board->combos[0].combo, 1);
    DrawCardsFromSet(&casina_Board->deck, &casina_Board->combos[1].combo, 1);

    DrawCardsFromSet(&casina_Board->deck, &casina_Board->player1.hand, CASINA_DRAW_COUNT);
    DrawCardsFromSet(&casina_Board->deck, &casina_Board->player2.hand, CASINA_DRAW_COUNT);
    DrawCardsFromSet(&casina_Board->deck, &casina_Board->combos[2].combo, 1);
    DrawCardsFromSet(&casina_Board->deck, &casina_Board->combos[3].combo, 1);

    casina_Board->combo_Count = 4;
}
