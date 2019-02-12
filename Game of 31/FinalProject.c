/*
Final Project
Kellan Loew (with Chris King)
12/5/2018
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

typedef struct card_s {  //This will act as a single node in a linked list
	char suit[9];
	int value;
	struct card_s *pt;
} card;

typedef struct {
	int playerWins;
	int dealerWins;
	int rounds;
	int player14Wins;
	int equals27;
} scoreboard;

card *addCardToTop(card *prevHead, char suit[], int value);
card *removeTopCard(card *head);
void moveCard(card **fromPile, card **toPile);
void destroyDeck(card *head);
card *createDecks();
int deckLength(card *head);
card *shuffleCards(card *head);
card *loadDecks(char fileName[]);
int getHandTotal(card *handHead);
void displayHand(card *head);
int playOneRound(card **deckHead, int *playerMoney, int minBet, int maxBet, scoreboard *sb);
void printCard(card *in);

int main() {

	card *deckHead = NULL; //These are all pointers to the head of the linked list (i.e. the top card in the deck or hand)
	scoreboard *sb = (scoreboard*)malloc(sizeof(scoreboard));  //creating a new scoreboard to keep track of statistics
	sb->dealerWins = 0;
	sb->equals27 = 0;
	sb->player14Wins = 0;
	sb->playerWins = 0;
	sb->rounds = 0;

	int playerMoney = 100, //Starting off with $100
		minBet = 5, //Setting up min/max bet variables for clarity in code and to make updates easier (if we wanted to change the rules of the game)
		maxBet = 20;

	int cardControl;
	char gameControl;
	char deckFile[] = "deck.txt";

	printf("Let's play a game of 31!\n");
	printf("Press 1 to shuffle the deck or 2 to load a deck from a file: ");
	scanf("%d", &cardControl);

	while (deckHead == NULL) {
		if (cardControl == 1) {		//Creates and shuffles a deck of cards
			deckHead = createDecks(); //Deck is created
			deckHead = shuffleCards(deckHead); //Deck is shuffled
			printf("The deck is shuffled.\n");
		}
		else { //Loads a pre-existing deck from file
			deckHead = loadDecks(deckFile);
			cardControl = 1; //Converting the deck load style to normal 
		}
	}

	gameControl = '1';
	while (gameControl != 'q' && playerMoney > 0) {

		if (deckLength(deckHead) < 30) { //Getting a new deck and shuffling it if we have less than 30 cards -> 2.2(m)
			destroyDeck(deckHead); //We have to get rid of the old deck first
			deckHead = createDecks(); //A new deck is created
			deckHead = shuffleCards(deckHead); //Deck is shuffled 
		}

		playOneRound(&deckHead, &playerMoney, minBet, maxBet, sb);

		if (playerMoney <= 0) {
			printf("You are out of money.  Thank you for playing.\n");
		}
		else {
			printf("Press any key to continue (or q to quit):");
			scanf(" %c", &gameControl);
		}
	}

	destroyDeck(deckHead);

	printf("GAME STATISTICS:\n\n");
	printf("Games played: %d\n", sb->rounds);
	printf("Player win percentage: %.1lf%%\n", (100 * (float)sb->playerWins / sb->rounds));
	printf("Player wins with 14: %.1lf%%\n", (100 * (float)sb->player14Wins / sb->rounds));
	printf("Dealer win percentage: %.1lf%%\n", (100 * (float)sb->dealerWins / sb->rounds));
	printf("Equal to 27 percentage: %.1lf%%\n", (100 * (float)sb->equals27 / sb->rounds));

	return 0;
}

card *addCardToTop(card *prevHead, char suit[], int value) {
	//Adds a new card to the deck/hand
	//prevHead will be where the new card points to.  Note, prevHead can be NULL if this is the first card added to the deck/hand
	//Utilizes malloc
	//Returns a pointer to the newly created card (which will be the new head)

	card *newCard = (card*)malloc(sizeof(card));  //allocating memory for the new card
	newCard->value = value; //setting the new card's value
	strcpy(newCard->suit, suit); //setting the new card's suit
	newCard->pt = prevHead;  //setting the new card's pointer to the previous card in the list. *Note* if NULL is passed in for prevHead, this will cause this card to be the first one in its respective list

	return newCard;
}

card *removeTopCard(card *head) {//** Unnecessary ** just do this step in moveCard (then only function that uses this)
								 //"Removes" the top card by returning the pointer stored in that card's struct
								 //Returns a pointer to the new head node
	return head->pt;
}

void moveCard(card **fromPile, card **toPile) {
	//Moves the top card (head) of the fromPile to the top (head) of the toPile and establishes the new head for the fromPile
	//Uses pointers to pointers, so there is no return value

	card *temp = *toPile;  //copies the toPile pointer (in effect copying the entire list)
	*toPile = *fromPile; //toPile copies the entire list (effectively) of fromPile

	*fromPile = removeTopCard(*fromPile); //fromPile has its top card removed

	(*toPile)->pt = temp; //the pointer of the top card of toPile (which at this point is still linking to the entire *original* fromPile) is moved over to the top card of the original toPile (essentially just adding a card to the top of toPile) 
}

void destroyDeck(card *head) {
	//Frees all memory associated with a given deck
	//No return value

	card *temp;  //We need a separate variable to keep track of the pt variable for each card so that we still have a reference to the next card after the current card is freed

	while (head != NULL) {//if head=NULL, we've reached the end of the list
		temp = head->pt; //retaining the pointer to the next card
		free(head); //freeing the memory for the current card
		head = temp; //turning the "next card" into the "current card"
	}
}

card *createDecks() {
	//Create two brand new ordered decks
	//Utilizes addCardToTop to add each new card
	//Returns a pointer to the head node (card)

	card *head = NULL;
	int i, suit, face;
	//This next section makes a double deck using addcardtotop function on each kind of card twice

	for (i = 0; i < 2; ++i) { //Loop runs twice to create two decks, one on top the other.
		for (suit = 0; suit < 4; ++suit) {		//Runs four times for each of the four suits
			for (face = 2; face < 15; ++face) { //Runs 13 times for each of the card values

				switch (suit) {
				case 0:  //Hearts
					head = addCardToTop(head, "Hearts", face);
					break;
				case 1:  //Spades
					head = addCardToTop(head, "Spades", face); //Creates all spade cards
					break;
				case 2:
					head = addCardToTop(head, "Diamonds", face); //Creates all diamonds cards
					break;
				case 3:
					head = addCardToTop(head, "Clubs", face); //Creates all clubs cards
					break;
				}
			}
		}
	}

	return head; //Return a pointer to the head node (card)
}

int deckLength(card *head) {
	int counter = 0;

	while (head != NULL) {
		counter++;
		head = head->pt;
	}

	return counter;
}

card *shuffleCards(card *head) {
	//Takes the decks of cards and shuffles the order of the cards
	//Returns a pointer to the head node (card)

	card *first;
	card *second;
	int random;

	int deckLen = deckLength(head); //Length of deck

	srand(time(0)); //Creates seeds for random numbers

	for (int i = 0; i < 500; ++i) { //Shuffles the cards i number of times

		random = 2 + rand() % (deckLen - 1); //This generates a number between 2 and the length of the deck.

		second = head;	//"Second" starts by pointing to top card of deck
		first = head->pt; //"First" starts by pointing the second to the top card

		while (--random > 1) { //Runs until "first" pointer points to the card in the deck that is specified by the random number
			second = first;  //moving the second pointer to point where first pointer is pointing
			first = first->pt;  //moving first pointer to point at the next card
		}

		second->pt = first->pt; //The pointer held in the "second" now points to the card "first" is pointing to, skipping over "first"
		first->pt = head; //The pointer held in the "first" now points to the head
		head = first; //The "first" now is the top of the deck
	}

	return head;
}

card *loadDecks(char fileName[]) {
	//Loads two decks of cards from a file
	//Utilizes addCardToTop to add each new card 
	//Returns a pointer to the head node (card)

	card *deckOut = NULL;
	char suit[9];
	int value;
	FILE* inp = NULL;
	inp = fopen(fileName, "r");

	if (inp == NULL) {
		printf("Sorry, deck file does not exist.");
	}
	else {
		while (fscanf(inp, "%d %s", &value, suit) != EOF) { //Reads value and suit from file
			deckOut = addCardToTop(deckOut, suit, value); //Card of above value and suit is added to deckOut
		}
	}
	return deckOut; //Returns a pointer to the head node (card)
}

int getHandTotal(card *handHead) { //Returns the sum of the card values in a hand, accounting for face cards and aces, as an int
	int total = 0, aceCount = 0;

	while (handHead != NULL) {
		if (handHead->value < 11) {//Normal card values
			total += handHead->value;
		}
		else if (handHead->value < 14) {//Face card values
			total += 10;
		}
		else {//Aces
			total += 11; //Adding the maximum amount for the ace and handling any overshoot later
			aceCount++;  //Incrementing the count of aces (to be used if there is overshoot)
		}

		handHead = handHead->pt;
	}

	while (total > 31 && aceCount > 0) { //If the total overshoots 31 AND we have one or more aces, we begin to reduce the total
		total -= 10;  //Reduce the total by 10 (turning an 11 ace into a 1 ace)
		aceCount--;  //Decrementing the number of aces we can use to reduce the total
	}

	return total;
}

void displayHand(card *head) {//** Can be made to be for extra credit
							  //Takes a card and displays it on the terminal using ASCII art
							  //Does not return a value

	while (head != NULL) { //looping through all the cards in a hand

						   //if (head->value < 11) { //normal (non-face) cards are easy
						   //	printf("%d of %s", head->value, head->suit);
						   //}
						   //else { //face cards have to be assigned string values
						   //	char face[10];

						   //	if (head->value == 11) strcpy(face, "Jack");
						   //	else if (head->value == 12) strcpy(face, "Queen");
						   //	else if (head->value == 13) strcpy(face, "King");
						   //	else strcpy(face, "Ace");

						   //	printf("%s of %s", face, head->suit);
						   //}

						   //if (head->pt != NULL) printf(", "); //Adding a coma to the printout for all but the last card

		printCard(head);

		head = head->pt; //moving the loop to the next card
	}

	printf("\n");
}

int playOneRound(card **deckHead, int *playerMoney, int minBet, int maxBet, scoreboard *sb) {
	//Plays one round of the game
	//Updates the player's money according to the outcome
	//Returns 0 if nobody wins, 1 if the player wins, 2 if the dealer wins

	int playerCont,
		playerTotal,
		dealerTotal,
		bet = 0,
		insuranceBet = 0;  //This will only change if the dealer gets an ace

	char getInsurance;

	card *playerHead = NULL,
		*dealerHead = NULL;

	//Placing bet before cards are dealt -> 2.2(d)
	while (bet < minBet || bet > maxBet || bet > *playerMoney) {//made this a loop to ensure bet is within bounds AND does not exceed playerMoney

		printf("Place your bet (must be between $%d and $%d): ", minBet, maxBet);
		scanf("%d", &bet); //assigning the bet amount to the bet variable

		if (bet < minBet) printf("Your bet is less than the minimum bet.  Please increase your bet.\n");
		else if (bet > maxBet) printf("Your bet exceeds the maximum bet.  Please lower your bet.\n");
		else if (bet > *playerMoney) printf("You do not have enough money to make that bet.  Please lower your bet to be below %d.\n", *playerMoney);
	}

	//Dealing cards in player-dealer order -> 2.2(e)
	moveCard(deckHead, &playerHead); //Player is dealt one card 
	printf("Your hand is \n\n");
	displayHand(playerHead);  //Printing player's hand
	playerTotal = getHandTotal(playerHead); //Getting the total value for the player's hand (or really, just the one card in this case)

	moveCard(deckHead, &dealerHead); //Dealer is dealt one card
									 //Dealer's card is dealt face up -> 2.2(f)
	printf("The dealer's card is \n\n");
	displayHand(dealerHead);
	dealerTotal = getHandTotal(dealerHead);

	if (dealerTotal == 11 && *playerMoney >= bet) {//Setting up a side bet for insurance if the dealer drew an ace and the player has enough money
		printf("The dealer drew an ace.  Would you like insurance? (y/n): \n");
		scanf(" %c", &getInsurance);

		if (getInsurance == 'y') insuranceBet = bet;
	}

	do {
		printf("Would you like to hit (1), stay (2), or double down (3)?\n");
		scanf("%d", &playerCont);

		if (playerCont == 1 || playerCont == 3) { //setting up an if statement in a do-while loop allows us to not have to ask the hit/stay question outside then inside the loop
			if (playerCont == 3) {
				if (*playerMoney >= 2 * bet) {//making sure the player has enough money to double down
					bet = 2 * bet;
				}
				else {
					printf("You do not have enough $$ to double down.  Sorry.");
					playerCont = 1;  //converting play-style back to normal play
				}
			}

			moveCard(deckHead, &playerHead);  //Player draws card

			printf("Your hand is now: \n\n");
			displayHand(playerHead);

			playerTotal = getHandTotal(playerHead);  //getting new total for player's hand

			if (playerTotal == 14) printf("You have 14.  If you stand, the dealer has to hit 31 to win.\n");
			else if (playerTotal > 31) printf("Your total is %d.  You have busted.\n", playerTotal);
			else printf("That makes your total %d\n", playerTotal);
		}
	} while (playerCont == 1 && playerTotal < 31);  //player's turn terminates when the player stands, hits 31, or busts -> 2.2(g)


	if (playerTotal <= 31) {//The dealer only gets cards if the player hasn't busted (not listed as a Game Rule, but a logical limitation)

		printf("\n\nDealer's turn\n\n");

		while (dealerTotal < 27 || (playerTotal == 14 && dealerTotal < 31)) { //Dealer takes hits until his hand totals 27 or more -> 2.2(i) OR in case the player has 14, dealer continues to draw until they hit 31 2.2(j)

			moveCard(deckHead, &dealerHead);  //Dealer draws card

			printf("The dealer's hand is now: \n\n");
			displayHand(dealerHead);

			dealerTotal = getHandTotal(dealerHead); //getting new total for dealer's hand

			if (dealerTotal > 31) printf("Dealer's total is %d.  The dealer has busted.\n", dealerTotal);
			else if (dealerTotal == 14 && playerTotal != 14) {//If the dealer gets 14, while the player does not have a 14, we break out of the loop
				printf("The dealer has 14 (automatic win).\n");
				break;
			}
			else printf("That makes the dealer's total %d\n", dealerTotal);
		}
	}

	//Now that we have the totals, we can deallocate the memory for the hands (before returning out of the function) -> 
	destroyDeck(playerHead);
	destroyDeck(dealerHead);

	//updating general game statistics
	sb->rounds++;
	if (playerTotal == 27) sb->equals27++;
	if (dealerTotal == 27) sb->equals27++;

	//This first condition should cover all of the cases where the player would lose **double check**
	//playerTotal > 31 --covers the player busting
	//dealerTotal == 14 --covers the dealer having 14, which is automatic win if player doesn't have 14 (case when player has 14 is handled in dealer while loop via continued draws for dealer)
	//(dealerTotal > playerTotal && dealerTotal <= 31 && playerTotal != 14) --standard case for dealer winning (dealer's total is higher than player's, but is not a bust AND the player's hand is not 14)
	//(playerTotal == 14 && dealerTotal == 31) --the one way for the dealer to beat a player's 14
	if (playerTotal > 31 || dealerTotal == 14 || (dealerTotal > playerTotal && dealerTotal <= 31) || (playerTotal == 14 && dealerTotal == 31)) {
		*playerMoney -= (bet + (dealerTotal == 31 ? -1 * insuranceBet : insuranceBet));  //if the dealer got 31, the insurance bet offsets the original bet (bet - insuranceBet), otherwise the loss is increased (bet + insuranceBet)
		printf("You lose :(\nYou now have $%d\n", *playerMoney);

		sb->dealerWins++; //updating dealer win count

		return 2;
	}
	else if (playerTotal == dealerTotal) {
		printf("Tie.  Nobody wins.\n");
		*playerMoney += dealerTotal == 31 ? insuranceBet : -1 * insuranceBet; //in the case where both the dealer and the player get 31, insurace will pay out.  Anything other than 31 (for the dealer) means the player loses the insurance bet

		return 0;
	}
	else {
		*playerMoney += (bet - insuranceBet); //We won't ever end up here if the dealer got 31.  So the only thing to do with the insurance bet is to subtract it from the winnings   
		printf("You win!\nYou now have $%d\n", *playerMoney);

		sb->playerWins++; //updating player win count
		if (playerTotal == 14) sb->player14Wins++; //updating counter for when the player wins with a 14

		return 1;
	}
}

void printCard(card *in) {
	char face;

	printf("\n_____________\n");

	if (in->value < 10) { //normal (non-face) cards are easy
		printf("| %d          |\n", in->value);
	}
	else if(in->value == 10) //One less space for two digits of 10
		printf("| %d         |\n", in->value);
	else { //face cards have to be assigned string values

		if (in->value == 11) face = 'J';
		else if (in->value == 12) face = 'Q';
		else if (in->value == 13) face = 'K';
		else face = 'A';

		printf("| %c          |\n", face);
	}

	if (strcmp(in->suit, "Hearts") == 0) {//Prints graphics
		printf("|    _  _    |\n");
		printf("|   ( \\/ )   |\n");
		printf("|    \\  /    |\n");
		printf("|     \\/     |\n");
	}
	else if (strcmp(in->suit, "Diamonds") == 0) {
		printf("|     /\\     |\n");
		printf("|    /  \\    |\n");
		printf("|    \\  /    |\n");
		printf("|     \\/     |\n");
	}
	else if (strcmp(in->suit, "Clubs") == 0) {
		printf("|      _     |\n");
		printf("|     ( )    |\n");
		printf("|    (_ _)   |\n");
		printf("|      |     |\n");
	}
	else {
		printf("|     /\\     |\n");
		printf("|    /  \\    |\n");
		printf("|   (_  _)   |\n");
		printf("|     /\\     |\n");
	}

	if (in->value < 10) { //normal (non-face) cards are easy
		printf("|__________%d_|\n", in->value);
	}
	else if (in->value == 10) { //One less space for two digits of 10
		printf("|_________%d_|\n", in->value);
	}
	else { //face cards have to be assigned string values

		if (in->value == 11) face = 'J';
		else if (in->value == 12) face = 'Q';
		else if (in->value == 13) face = 'K';
		else face = 'A';

		printf("|__________%c_|\n", face);
	}
}