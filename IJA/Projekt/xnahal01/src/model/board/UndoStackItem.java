package model.board;

import model.cards.CardDeck;
import model.cards.CardStack;

import java.awt.*;
import java.io.Serializable;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * <p>
 * Trieda reprezentujúca položku v zásobníku vykonaných ťahov.
 * Využíva sa pri operáci Undo.
 */

public class UndoStackItem implements Serializable {

    private CardDeck cardDeckFrom;
    private CardDeck cardDeckTo;
    private int size;
    private Container graphicFrom;
    private Container graphicTo;
    private int score;
    private boolean rotate;
    private boolean one_card;

    /**
     * @return Skóre hry.
     */
    public int getScore() {
        return score;
    }

    /**
     * @return Balíček kariet, z ktorého sme zobrali kartu.
     */
    public CardDeck getCardDeckFrom() {
        return cardDeckFrom;
    }

    /**
     * @return Balíček kartiet, na ktorý sme položili kartu.
     */
    public CardDeck getCardDeckTo() {
        return cardDeckTo;
    }

    /**
     * @return Veľkosť presúvaného balíčku - v prípade že presúvame viac ako jednu kartu.
     */
    public int getSize() {
        return size;
    }

    /**
     * @return Grafický objekt odkiaľ berieme kartu.
     */
    public Container getGraphicFrom() {
        return graphicFrom;
    }

    /**
     * @return Grafický objekt, kam pokladáme kartu.
     */
    public Container getGraphicTo() {
        return graphicTo;
    }

    /**
     * @return True, ak sa karta ktorá ležala pod presúvanou kartou otočila.
     */
    public boolean isRotate() {
        return rotate;
    }

    /**
     * @return True, ak sme presúvali jednu kartu
     */
    public boolean isOne_card() {
        return one_card;
    }

    /**
     * Konštruktor jedného vykonaného ťahu, ak presúvame viacero kariet.
     * @param deckFrom Balíček kariet, z ktorého sme zobrali karty.
     * @param deckTo Balíček kartiet, na ktorý sme položili karty.
     * @param grFrom Grafický objekt, odkiaľ berieme karty.
     * @param grTo Grafický objekt, kam pokladáme karty.
     * @param size Počet presúvaných kariet.
     * @param rot True, ak sa karta ktorá ležala pod presúvanými kartami otočila.
     * @param score Skóre hry v čase vykonania ťahu.
     */
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

    /**
     * Konštruktor jedného vykonaného ťahu, ak presúvame jednu kartu.
     * @param deckFrom Balíček kariet, z ktorého sme zobrali kartu.
     * @param deckTo Balíček kartiet, na ktorý sme položili kartu.
     * @param grFrom Grafický objekt, odkiaľ berieme kartu.
     * @param grTo Grafický objekt, kam pokladáme kartu.
     * @param rot True, ak sa karta ktorá ležala pod presúvanou kartami otočila.
     * @param score Skóre hry v čase vykonania ťahu.
     */
    public UndoStackItem(CardDeck deckFrom, CardDeck deckTo, Container grFrom, Container grTo, boolean rot, int score){
        this.cardDeckFrom = deckFrom;
        this.cardDeckTo = deckTo;
        this.graphicFrom = grFrom;
        this.graphicTo = grTo;
        this.rotate = rot;
        this.score = score;
        this.one_card = true;
    }
}
