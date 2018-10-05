package model.cards;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * <p>
 * Trieda implementujúca metódy pre prácu s balíčkom kariet (deck).
 */

public class CardDeckKlondike implements CardDeck, Serializable {

    private ArrayList<Card> deck = new ArrayList<>();
    private int size;
    private Card.Color color;

    public CardDeckKlondike() {
    }

    @Override
    public boolean isEmpty() {
        return (this.size == 0);
    }

    @Override
    public int size() {
        return this.size;
    }

    @Override
    public Card get() {
        if (this.isEmpty())
            return null;
        else
            return getDeck().get(size - 1);
    }

    @Override
    public Card get(int index) {
        if (index > this.size || index < 0)
            return null;
        else
            return getDeck().get(index);
    }

    @Override
    public Card pop() {

        if (this.isEmpty())
            return null;

        Card newCard;
        newCard = getDeck().get(size - 1);
        getDeck().remove(size - 1);
        this.size--;
        return newCard;
    }

    /**
     * Overí, či je možné vykonať vloženie karty na balíček.
     * Karta musí mať zhodnú farbu a o 1 väčšiu hodnotu.
     *
     * @param card Vkládaná karta.
     * @return Úspešnosť operácie - true ak je možné kartu vložiť.
     */
    @Override
    public boolean checkPut(Card card) {
        if (card.value() != this.size() + 1)
            return false;

        if (this.color != null) {
            if (this.color != card.color())
                return false;
        }
        return true;
    }

    @Override
    public boolean rotateLastCard() {
        Card c = this.pop();
        if (c != null) {
            this.put(c.color(), c.value());
            return true;
        }
        return false;
    }

    /**
     * Vloží kartu na cieľový deck. Karta musí mať zhodnú farbu a o 1 väčšiu hodnotu
     *
     * @param card Vkládaná karta.
     * @return uspešnosť operácie
     */
    @Override
    public boolean put(Card card) {
        if (!checkPut(card))
            return false;
        else
            this.color = card.color();

        this.getDeck().add(card);
        this.size++;
        return true;
    }

    @Override
    public void put(Card.Color c, int value) {
        this.getDeck().add(new CardKlondike(c, value));
        this.size++;
    }

    @Override
    public void shuffleCards() {
        Collections.shuffle(this.getDeck());
        Collections.shuffle(this.getDeck());
        Collections.shuffle(this.getDeck());
        Collections.shuffle(this.getDeck());
    }

    @Override
    public ArrayList<Card> getDeck() {
        return deck;
    }


}
