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

public class CardDeckKlondike implements CardDeck, Serializable {

    private ArrayList<Card> deck = new ArrayList<>();
    private int size;
    private Card.Color color;

    public CardDeckKlondike() { }

    public CardDeckKlondike(Card.Color color) {
        this.color = color;
    }

    @Override
    public boolean isEmpty(){
        return (this.size == 0);
    }

    @Override
    public int size(){
//        System.err.println("size2");
        return this.size;
    }

    @Override
    public Card get(){
        if(this.isEmpty())
            return null;
        else
            return getDeck().get(size-1);
    }

    @Override
    public Card get(int index){
        if(index > this.size || index < 0)
            return null;
        else
            return getDeck().get(index);
    }

    @Override
    public Card pop(){

        if(this.isEmpty())
            return null;

        Card newCard ;
        newCard = getDeck().get(size-1);
        getDeck().remove(size-1);
        this.size--;
        return newCard;
    }

    @Override
    public boolean checkPut(Card card){
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
        if (c != null) {
            c.rotateCard();
            this.put(c.color(), c.value());
            return true;
        }
        return false;
    }

    @Override
    public void forceRotate() {
        Card c = this.get();
        c.rotateCard();
    }

    /**
     * Vloží kartu na cieľový deck. Karta musí mať zhodnú farbu a o 1 väčšiu hodnotu
     *
     * @param card Vkládaná karta.
     * @return uspešnosť operácie
     */
    @Override
    public boolean put(Card card){
        if(!checkPut(card))
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
    public void shuffleCards(){
        //long seed = System.nanoTime();
        //Collections.shuffle(this.deck, new Random(seed));
        Collections.shuffle(this.getDeck());
        Collections.shuffle(this.getDeck());
        Collections.shuffle(this.getDeck());
        Collections.shuffle(this.getDeck());
    }

    /**
     * @return the deck
     */
    @Override
    public ArrayList<Card> getDeck()
    {
        return deck;
    }
    


}
