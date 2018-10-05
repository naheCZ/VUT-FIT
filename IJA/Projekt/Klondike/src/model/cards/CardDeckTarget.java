/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package model.cards;

import java.io.Serializable;
import java.util.ArrayList;

/**
 *
 * @author Roman
 */
public class CardDeckTarget implements CardDeck, Serializable
{
    private ArrayList<Card> cards;
    private Card.Color color;


    public CardDeckTarget()
    {
        this.cards = new ArrayList<Card>();
        //this.color = color;
    }

    @Override
    public int size()
    {
       // System.err.println("size1");
        return this.getCards().size();
    }

    @Override
    public boolean put(Card card) {

        if(!checkPut(card))
            return false;

        this.cards.add(card);
        this.color = card.color();
        return true;

    }

    @Override
    public Card pop()
    {
        int size = this.getCards().size();
        Card removed;

        if(size == 0)
            return null;

        removed = this.getCards().remove(size - 1);

        return removed;
    }

    @Override
    public Card get()
    {
        int size = this.getCards().size();

        if(size == 0)
            return null;

        else
            return this.getCards().get(size - 1);
    }

    @Override
    public Card get(int index)
    {
        int size = this.getCards().size();

        if((index < 0) || index > (size - 1))
            return null;

        else
            return this.getCards().get(size - 1);
    }

    @Override
    public boolean isEmpty()
    {
        return this.getCards().isEmpty();
    }

    public ArrayList<Card> getCards()
    {
        return cards;
    }

    public Card.Color getColor()
    {
        return color;
    }

    @Override
    public void put(Card.Color c, int value) {
        this.cards.add(new CardKlondike(c, value));
        this.color = c;
    }

    @Override
    public void shuffleCards() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public boolean checkPut(Card card){
        boolean empty = this.isEmpty();

        if(empty && card.value() == 1)
            return true;
        
        if( card.value() != this.size() + 1)
            return false;

        if(this.color != null) {
            if (this.color != card.color())
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
        Card c = this.get();
        c.rotateCard();
    }

    @Override
    public ArrayList<Card> getDeck()
    {
        return this.cards;
    }
}
