package model.cards;

import java.io.Serializable;

/**
 * project: project Solitaire model.Klondike - IJA 2016/2017
 * authors: Adam Zivcak, Roman Nahalka
 * login:   xzivca03, xnahal01
 * created: 30.3.2017
 */
public class CardKlondike implements Card, Serializable {

    private Color color;
    private int value;
    private boolean isTurnedFaceUp;
    private String img;

    public CardKlondike(Card.Color c, int value) {
        this.color = c;
        if (value < 1 || value > 13)
            throw new RuntimeException("Wrong card value. Must be between 1 and 13.");
        else
            this.value = value;
    }

    public int value() {
        return this.value;
    }

    public Card.Color color() {
        return this.color;
    }

    public Card.ColorGroup getColorGroup(){
        if (this.color == Color.CLUBS || this.color == Color.SPADES)
            return ColorGroup.BLACK;
        else
            return ColorGroup.RED;
    }

    public boolean similarColorTo(Card c){
        return (this.getColorGroup() == c.getColorGroup());
    }

    public int compareValue(Card c){
        if(this.value() == c.value())
            return 0;
        else
            return (this.value() - c.value());
    }

    public boolean isTurnedFaceUp(){
        return this.isTurnedFaceUp;
    }

    public boolean turnFaceUp(){
        if (this == null)
            return false;
        if(this.isTurnedFaceUp)
            return false;
        else {
            this.isTurnedFaceUp = true;
            return true;
        }
    }

    // vracia true ak sa otočenie uskutočnilo
    public void rotateCard(){}
    /*
        if(this.isTurnedFaceUp())
            this.isTurnedFaceUp = false;
        else
            this.isTurnedFaceUp = true;
    }*/

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        CardKlondike t = (CardKlondike) o;

        if (value != t.value) return false;
        return color == t.color;
    }

    @Override
    public int hashCode() {
        int result = color.hashCode();
        result = 31 * result + value;
        return result;
    }


    @Override
    public String toString() {
        int val = this.value;
        Color c = this.color;

        String text;

        switch (val) {
            default:
                text = val + "(" + c.toString() + ")";
                break;
            case 1:
                text = 'A' + "(" + c.toString() + ")";
                break;
            case 11:
                text = 'J' + "(" + c.toString() + ")";
                break;
            case 12:
                text = 'Q' + "(" + c.toString() + ")";
                break;
            case 13:
                text = 'K' + "(" + c.toString() + ")";
        }
        return text;
    }


    public String toString2() {
        int val = this.value;
        Color c = this.color;

        String text;

        switch (val) {
            default:
                text = val + "(" + c.toString() + ")" + this.isTurnedFaceUp;
                break;
            case 1:
                text = 'A' + "(" + c.toString() + ")"+ this.isTurnedFaceUp;
                break;
            case 11:
                text = 'J' + "(" + c.toString() + ")"+ this.isTurnedFaceUp;
                break;
            case 12:
                text = 'Q' + "(" + c.toString() + ")"+ this.isTurnedFaceUp;
                break;
            case 13:
                text = 'K' + "(" + c.toString() + ")"+ this.isTurnedFaceUp;
        }
        return text;
    }
    
    public String getImage()
    {
        return "/images/" + this.toString() + ".png";
    }

}
