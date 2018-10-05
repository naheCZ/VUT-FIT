package model.cards;

import java.io.Serializable;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 *
 * Trieda implementujúca metódy pre prácu s kartou.
 */

public class CardKlondike implements Card, Serializable {

    private Color color;
    private int value;
    private boolean isTurnedFaceUp;

    public CardKlondike(Card.Color c, int value) {
        this.color = c;
        if (value < 1 || value > 13)
            throw new RuntimeException("Wrong card value. Must be between 1 and 13.");
        else
            this.value = value;
    }

    @Override
    public int value() {
        return this.value;
    }

    @Override
    public Card.Color color() {
        return this.color;
    }

    @Override
    public Card.ColorGroup getColorGroup() {
        if (this.color == Color.CLUBS || this.color == Color.SPADES)
            return ColorGroup.BLACK;
        else
            return ColorGroup.RED;
    }

    @Override
    public boolean similarColorTo(Card c) {
        return (this.getColorGroup() == c.getColorGroup());
    }

    @Override
    public boolean isTurnedFaceUp() {
        return this.isTurnedFaceUp;
    }

    @Override
    public boolean turnFaceUp() {
        if (this.isTurnedFaceUp)
            return false;
        else {
            this.isTurnedFaceUp = true;
            return true;
        }
    }

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

    @Override
    public String getImage() {
        return "/images/" + this.toString() + ".png";
    }

}
