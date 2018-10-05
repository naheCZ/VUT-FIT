package model.cards;


import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 *
 * Trieda implementujúca metódy pre prácu s pracovným balíčkom kariet (stack).
 */

public class CardStackKlondike implements CardStack, Serializable {

    private ArrayList<Card> stack = new ArrayList<>();
    private int size;

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
            return this.stack.get(size - 1);
    }

    @Override
    public Card get(int index) {
        if (index > this.size || index < 0)
            return null;
        else
            return this.stack.get(index);
    }

    @Override
    public Card pop() {
        if (this.isEmpty())
            return null;

        Card newCard = this.stack.get(this.size - 1);
        this.stack.remove(this.size - 1);
        this.size--;
        return newCard;
    }

    @Override
    public CardStack pop(Card card) {
        int idxFrom = -1;

        for (int i = this.size - 1; i >= 0; i--) {
            if (this.stack.get(i).equals(card)) {
                idxFrom = i;
                break;
            }
        }
        if (idxFrom <= -1)
            return null;

        int newSize = this.size - idxFrom;
        CardStackKlondike newStack = new CardStackKlondike();

        for (int i = 0; i < newSize; i++) {
            newStack.stack.add(this.stack.get(idxFrom));
            newStack.size++;
            this.size--;
            this.stack.remove(this.stack.get(idxFrom));
        }
        return newStack;
    }

    @Override
    public boolean put(CardStack stack) {
        CardStackKlondike tmpStack = new CardStackKlondike();

        for (int j = 0; j < stack.size(); j++) {
            tmpStack.stack.add(j, stack.get(stack.size() - j - 1));
            tmpStack.size++;
        }

        while (tmpStack.size > 0) {
            if (!this.put(tmpStack.pop()))
                return false;
        }

        for (int j = 0; j < stack.size(); j++) {
            stack.pop();
        }

        return true;
    }

    /**
     * Overí, či je možné vykonať vloženie karty na balíček.
     * Karta musí mať rozdielnú farbu a o 1 mänšiu hodnotu.
     * Na prázdy balíček je možné vložiť len kráľa.
     *
     * @param card Vkládaná karta.
     * @return Úspešnosť operácie - true ak je možné kartu vložiť.
     */
    @Override
    public boolean checkPut(Card card) {
        if (this.size == 0) {
            if (card.value() != 13)
                return false;
        } else {
            if (this.stack.get(this.size - 1).value() - 1 != card.value())
                return false;

            if (this.stack.get(this.size - 1).similarColorTo(card))
                return false;
        }
        return true;
    }

    @Override
    public boolean rotateLastCard() {
        Card c = this.pop();
        this.put(c.color(), c.value());
        return true;
    }

    @Override
    public boolean put(Card card) {
        if (!this.checkPut(card))
            return false;

        this.stack.add(card);
        this.size++;
        return true;
    }

    @Override
    public void put(Card.Color c, int value) {
        this.stack.add(new CardKlondike(c, value));
        this.size++;
    }

    @Override
    public void shuffleCards() { }

    @Override
    public ArrayList<Card> getDeck() {
        return this.stack;
    }
}
