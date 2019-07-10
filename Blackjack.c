/*
	Alex Pacheco
	COMP 340 - Homework 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//STRUCT DEFINITIONS
struct Card
{
	int value;
	char* name;
	char* suit;
};

struct Hand
{
	struct Card cards[21];
	int numberCards;
};

//GLOBAL VARIABLES
const int NUM_DECKS = 6;
struct Card deck[312];		// NUM_DECKS * 52
int topOfDeck = 311;
int playerChips = 1000;

//FUNCTION PROTOYPES
void createDeck();
char* determineCardName(int cardNumber);
char* determineCardSuit(int cardNumber);
int determineCardValue(int cardNumber);
void shuffleDeck();
int getHandValue(struct Hand hand);
void playRound();
int getBetFromPlayer();
struct Card drawCard();
int playerHit(struct Hand playerHand);		//return 0 for false, 1 for true

int main()
{
	srand(time(NULL));
	printf("\nCOMP 340 Spring 2018 - Homework 2\n");
	
	printf("\n\nBLACKJACK!\n");
	printf("Beat the Dealer to win chips\n");
	printf("Get 21 for an extra payout\n");
	printf("Ace         -  1 or 11\n");
	printf("Face cards  -  10\n");
	printf("All others  -  Pip value\n");

	//create and shuffle a deck of cards
	createDeck();
	shuffleDeck();
	
	int continuePlaying = 0;		//0 for false, 1 for true

	do
	{
		//if there are few cards remaining, a new deck is created
		if(topOfDeck < 60)
		{
			createDeck();
			shuffleDeck();
			printf("Resetting deck...\n");
		}

		//play one round of Blackjack
		playRound();
		
		//check if game should continue
		if(playerChips > 0)
			continuePlaying = 1;
		else
			continuePlaying = 0;

	} while(continuePlaying != 0);		//0 for false, 1 for true

	//loop is broken when player has no chips available, meaning the game is over
	printf("GAME OVER\n");
}

void printRow(int rowNumber)
{
	int x = 0;
	for(x = 0; x < 8 - rowNumber; x++)
	{
		printf(" ");
	}
	
	for(x = 0; x < rowNumber; x++)
	{
		printf("%d", (rowNumber + x) % 10);
	}
	
	for(x = rowNumber - 1; x > 0; x--)
	{
		printf("%d", (rowNumber + x - 1) % 10);
	}
	
	printf("\n");
}

void createDeck()
{
	//for each deck (typical Blackjack games use multiple decks)
	for(int deckNumber = 0; deckNumber < NUM_DECKS; deckNumber++)
	{
		//make all cards in a typical 52-card deck
		for(int cardNumber = 0; cardNumber < 52; cardNumber++)
		{
			//make new card
			int index = (deckNumber*52) + (cardNumber+1) - 1;
			deck[index].value = determineCardValue(cardNumber);
			deck[index].suit = determineCardSuit(cardNumber);
			deck[index].name = determineCardName(cardNumber);
			
			//print all cards
			/*
			if(n == 0)
			{
				printf("%s %s - %d\n",
					deck[(deckNumber + 1) * cardNumber].name,
					deck[(deckNumber + 1) * cardNumber].suit,
					deck[(deckNumber + 1) * cardNumber].value);
			}
			*/
			
			// j -> value suit
			// 0 -> A spades
			// 1 -> 2 hearts
			// 13 -> A hearts
			// 38 -> K clubs
			// 39 -> A diamonds
			// 51 -> K diamonds
		}
	}
	
	topOfDeck = NUM_DECKS * 52 - 1;
}

char* determineCardName(int cardNumber)
{
	//Ace can be 1 or 11, here it is assigned 1
	//face cards (J, Q, K) are 10
	//all other cards are pip value
	
	switch(cardNumber % 13 + 1)
	{
		case 1: return "A";
		case 11: return "J";
		case 12: return "Q";
		case 13: return "K";
	}
	
	char* str = malloc(8);
	sprintf(str, "%d", (cardNumber % 13 + 1));		//pip value
	return str;
}

char* determineCardSuit(int cardNumber)
{
	switch(cardNumber % 4)
	{
		case 0: return "spades";
		case 1: return "hearts";
		case 2: return "clubs";
		case 3: return "diamonds";
		default: return "";
	}
}

int determineCardValue(int cardNumber)
{
	switch(cardNumber % 13 + 1)
	{
		case 11: return 10;
		case 12: return 10;
		case 13: return 10;
		default: return cardNumber % 13 + 1;
	}
}

void shuffleDeck()
{
	//switch card places 100000 (a bunch of) times
	for(int x = 0; x < 100000; x++)
	{
		//pick random card to be switched
		int index1 = rand() % (NUM_DECKS * 52);
		int index2 = rand() & (NUM_DECKS * 52);

		//temporarily store the cards
		struct Card c1 = deck[index1];
		struct Card c2 = deck[index2];

		//switch the cards
		deck[index1] = c2;
		deck[index2] = c1;
	}

	//PROOF OF SHUFFLED DECK
	/*
	for(int x = 0; x < (NUM_DECKS * 52); x++)
	{
		printf("%s %s\n", deck[x].name, deck[x].suit);
	}
	*/
}

int getHandValue(struct Hand hand)
{
	//Aces can be 1 or 11

	int totalValue = 0;
	int numAces = 0;

	//for every card in the hand
	for(int x = 0; x < hand.numberCards; x++)
	{
		struct Card c = hand.cards[x];
	
		//if card is an Ace, keep track of it but do not add value
		if(c.name == "A")
		{
			numAces++;
		}
		//add value to total for all other cards
		else
		{
			totalValue += c.value;
		}
	}

	//checks how many Aces have to be added as 1
	//starts by assuming all Aces are 11, then removes
	//one at a time until all Aces are determined to
	//be 1 or 11
	for(int x = numAces; x > 0; x--)
	{
		//if making all remaining Aces count as 11 would keep
		//player under/equal 21, add them to total value as 11
		if(totalValue + 11*x <= 21)
		{
			totalValue += 11;
		}
		//if making all remaining Aces count as 11 would cause
		//player to bust, treat this one Ace as 1 and continue loop
		else
		{
			totalValue += 1;
		}
	}

	return totalValue;
}

void playRound()
{
	int bet = 0;
	struct Hand dealerHand;
	dealerHand.numberCards = 0;
	struct Hand playerHand;
	playerHand.numberCards = 0;
	
	printf("\n--- --- --- ROUND START --- --- ---\n");

	//collect bet from player
	bet = getBetFromPlayer();

	//cards are distributed to dealer
	dealerHand.cards[dealerHand.numberCards] = drawCard();
	dealerHand.numberCards++;
	dealerHand.cards[dealerHand.numberCards] = drawCard();
	dealerHand.numberCards++;
	
	//first card in the dealer's hand displayed to player
	printf("Dealer Face Up Card: %s %s\n\n", dealerHand.cards[0].name, dealerHand.cards[0].suit);
	
	printf("Your cards:\n");

	//player gets their cards
	playerHand.cards[0] = drawCard();
	playerHand.numberCards++;
	playerHand.cards[1] = drawCard();
	playerHand.numberCards++;
	printf("%s %s\n", playerHand.cards[0].name, playerHand.cards[0].suit);
	printf("%s %s\n", playerHand.cards[1].name, playerHand.cards[1].suit);
	
	//while a player continues to hit, they receive another card
	while(playerHit(playerHand) != 0 && playerHand.numberCards < 21)
	{
		playerHand.cards[playerHand.numberCards] = drawCard();
		printf("%s %s\n", playerHand.cards[playerHand.numberCards].name,
			playerHand.cards[playerHand.numberCards].suit);
		playerHand.numberCards++;
	}
	//when player no longer wants cards, they stand and their turn end

	//dealer hole card is displayed
	printf("\nDealer Hole Card: %s %s\n", dealerHand.cards[1].name, dealerHand.cards[1].suit);

	//dealer takes turn hitting or standing
	while(getHandValue(dealerHand) < 17)
	{
		struct Card nextCard = drawCard();
		dealerHand.cards[dealerHand.numberCards] = nextCard;
		dealerHand.numberCards++;
		printf("Dealer hits. %s %s\n", nextCard.name, nextCard.suit);
	}

	printf("Dealer Hand Value: %d\n\n", getHandValue(dealerHand));

	//each player is paid out any potential winnings
	//store current player, bet, and player's hand value
	int playerHV = getHandValue(playerHand);
	int dealerHV = getHandValue(dealerHand);

	//player loses chips if they bust or get less than dealer (assuming dealer doesn't bust)
	if(playerHV > 21 || (playerHV < dealerHV && dealerHV <= 21))
	{
		playerChips += -bet;
		printf("You lost %d chips.\n", bet);
	}
	//player wins extra chips for getting 21
	else if(playerHV == 21)
	{
		playerChips += (3*bet/2);
		printf("You won %d chips.\n", 3*bet/2);
	}
	//bet is pushed if player matches dealer
	else if(playerHV == dealerHV)
	{
		printf("You do not win or lose chips.\n");
	}
	//player wins chips for beating dealer and staying under 21
	else
	{
		playerChips += bet;
		printf("You won %d chips.\n", bet);
	}
	
	printf("\n");
}

int getBetFromPlayer()
{
	int numChips = 0;

	//do not let player wager 0 chips or more chips than they have available
	do
	{
		printf("Player, enter amount to bet (%d chips available): ", playerChips);
		
		char str[10];
		memset(str, '\0', 10*sizeof(char));
		fgets(str, 10, stdin);

		numChips = atoi(str);
		
	} while(numChips == 0 || numChips > playerChips);

	return numChips;
}

struct Card drawCard()
{
	struct Card c = deck[topOfDeck];
	topOfDeck--;
	return c;
}

int playerHit(struct Hand playerHand)		//return 0 for false, 1 for true
{
	char str[10];
	memset(str, '\0', 10*sizeof(char));

	//if the user already has 21, do not let them hit
	if(getHandValue(playerHand) == 21)
	{
		printf("Blackjack!\n");
		return 0;
	}
	//if the user has busted, do not let them hit
	else if(getHandValue(playerHand) > 21)
	{
		printf("Bust!\n");
		return 0;
	}
	//let the user hit if they choose to
	else
	{
		printf("Enter '1' to Hit, any key to Stand: ");
		fgets(str, 10, stdin);

		if(atoi(str) == 1)
			return 1;
		else
			return 0;
	}
}
