package model.cards;

/**
 * project: project Solitaire model.Klondike - IJA 2016/2017
 * authors: Adam Zivcak, Roman Nahalka
 * login:   xzivca03, xnahal01
 * created: 30.3.2017
 */

public interface Card {

    /**
     * Porovná hodnotu karty se zadanou kartou c.
     * Pokud jsou stejné, vrací 0.
     * Pokud je karta větší než zadaná c, vrací kladný rozdíl hodnot.
     * @param c Karta, s kterou se porovnává.
     * @return Vrací rozdíl hodnot karet.
     */
    int compareValue(Card c);

    /**
     * Testuje, zda je karta otočená lícem nahoru.
     *
     * @return Výsledek testu: true = karta je otočená lícem nahoru.
     */
    boolean isTurnedFaceUp();

    /**
     * Testuje, zda má karta podobnou barvu jako karta zadaná.
     * Podobnou barvou se myslí černá (piky, kříže) a červená (káry a srdce).
     * @param c - Karta pro porovnání.
     * @return Informace o shodě barev karet.
     */
    boolean similarColorTo(Card c);

    /**
     * Otočí kartu lícem nahoru.
     * @return Informace, zda došlo k otočení karty (=true) nebo ne.
     */
    boolean turnFaceUp();


    /**
     * @return Hodnota karty.
     */
    int value();

    /**
     * @return Farbu karty
     */
    Card.Color color();

    Card.ColorGroup getColorGroup();
    
    String getImage();

    void rotateCard();


    enum Color {
        CLUBS("C"),
        DIAMONDS("D"),
        HEARTS("H"),
        SPADES("S");
        private String text;

        // konstruktor pre enum hodnoty
        Color(String text) {
            this.text = text;
        }

        @Override
        public String toString() {
            return text;
        }
    }

    enum ColorGroup {
        BLACK("B"),
        RED("R");
        private String text;

        // konstruktor pre enum hodnoty
        ColorGroup(String text) {
            this.text = text;
        }

       @Override
        public String toString() {
            return text;
        }
    }

    String toString2();


}