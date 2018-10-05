package model.board;

import model.cards.*;

import java.io.Serializable;

/**
 * project: project Solitaire model.Klondike - IJA 2016/2017
 * authors: Adam Zivcak, Roman Nahalka
 * login:   xzivca03, xnahal01
 * created: 30.3.2017
 */

public class FactoryKlondike extends AbstractFactorySolitaire implements Serializable {


    @Override
    public CardDeck createCardDeck() {
        CardDeck newDeck = new CardDeckKlondike();
        int i = 0;
/*
        for (Card.Color color : Card.Color.values()) {
            for (int j = 1; j <= 13; i++, j++) {
                newDeck.put(color, j);
            }
        }
*/
        for (Card.Color color : Card.Color.values()) {
            for (int j = 13; j >= 1; i++, j--) {

                newDeck.put(color, j);
            }
        }

        return newDeck;
    }

    @Override
    public Card createCard(Card.Color color, int value) {
        if (value < 1 || value > 13) {
            return null;
        } else {
            return new CardKlondike(color, value);
        }
    }

    @Override
    public CardDeckTarget createTargetPack(Card.Color color) {
        return new CardDeckTarget();
    }

    @Override
    public CardStack createWorkingPack() {
        return new CardStackKlondike();
    }



}


