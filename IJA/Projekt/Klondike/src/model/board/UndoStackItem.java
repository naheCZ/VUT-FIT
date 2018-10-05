package model.board;

import model.cards.CardDeck;
import model.cards.CardStack;

import java.awt.*;
import java.io.Serializable;

/**
 * project: Klondike (copy)
 * author: Adam Zivcak
 * login:  xzivca03
 * created: 22.4.2017
 */

// undostack - položka v stakcu
public class UndoStackItem implements Serializable {

    private CardDeck cardDeckFrom;
    private CardDeck cardDeckTo;
    private int size;
    private Container graphicFrom;
    private Container graphicTo;
    private int score;
    private boolean rotate;
    private boolean one_card;

    public int getScore() {
        return score;
    }

    public CardDeck getCardDeckFrom() {
        return cardDeckFrom;
    }

    public CardDeck getCardDeckTo() {
        return cardDeckTo;
    }

    public int getSize() {
        return size;
    }

    public Container getGraphicFrom() {
        return graphicFrom;
    }

    public Container getGraphicTo() {
        return graphicTo;
    }

    public boolean isRotate() {
        return rotate;
    }

    public boolean isOne_card() {
        return one_card;
    }

    public UndoStackItem(CardDeck deckFrom, CardDeck deckTo, Container grFrom, Container grTo, int size, boolean rot, int score){
        this.cardDeckFrom = deckFrom;
        this.cardDeckTo = deckTo;
        this.graphicFrom = grFrom;
        this.graphicTo = grTo;
        this.size = size;
        this.rotate = rot;
        this.score = score;
        this.one_card = false;
    }

    public UndoStackItem(CardDeck deckFrom, CardDeck deckTo, Container grFrom, Container grTo, boolean rot, int score){
        this.cardDeckFrom = deckFrom;
        this.cardDeckTo = deckTo;
        this.graphicFrom = grFrom;
        this.graphicTo = grTo;
        this.rotate = rot;
        this.score = score;
        this.one_card = true;
    }




    public UndoStackItem getUndoStackItem(){
        return this;
    }
}
