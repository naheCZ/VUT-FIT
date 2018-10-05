package model.cards;


import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;

/**
 * project: project Solitaire model.Klondike - IJA 2016/2017
 * authors: Adam Zivcak, Roman Nahalka
 * login:   xzivca03, xnahal01
 * created: 30.3.2017
 */

public class CardStackKlondike implements CardStack, Serializable {

    private ArrayList<Card> stack = new ArrayList<>();
    private int size;

    public boolean isEmpty() {
        return (this.size == 0);
    }

    public int size() {
        //        System.err.println("size3");
        return this.size;
    }

    public Card get() {
        if (this.isEmpty())
            return null;

        else
            return this.stack.get(size - 1);
    }

    public Card get(int index) {

        if (index > this.size || index < 0)
            return null;

        else
            return this.stack.get(index);
    }

    public Card pop() {

        if (this.isEmpty())
            return null;

        Card newCard = this.stack.get(this.size - 1);
        this.stack.remove(this.size - 1);
        this.size--;
        return newCard;
    }

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

    public boolean put2(CardStack stack) {

        CardStackKlondike tmpStack = new CardStackKlondike();

        for (int j = 0; j < stack.size(); j++) {
            tmpStack.stack.add(j, stack.get(stack.size() - j - 1));
            tmpStack.size++;
        }

        while (tmpStack.size > 0) {
            this.put(tmpStack.get().color(), tmpStack.pop().value());
        }

        for (int j = 0; j < stack.size(); j++) {
            stack.pop();
        }

        return true;
    }

    public boolean checkPut(Card card){
        if (this.size == 0) {
            if (card.value() != 13)                   // na prazdny zasobnik môžeme vložiť len krala
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
        c.rotateCard();
        this.put(c.color(), c.value());
        return true;
    }

    @Override
    public void forceRotate() {
        this.get().rotateCard();
    }


    public boolean put(Card card) {

        if (!this.checkPut(card))
            return false;

        this.stack.add(card);
        this.size++;
        return true;
    }

    public void put(Card.Color c, int value) {
        this.stack.add(new CardKlondike(c, value));
        this.size++;
    }

    public void shuffleCards(){
        Collections.shuffle(this.stack);
    }

    @Override
    public ArrayList<Card> getDeck()
    {
        return this.stack;
    }
}
