package model.cards;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * <p>
 * Rozhranie pre prácu s kartou.
 */

public interface Card {

    /**
     * Testuje, či je karta otočená lícom hore.
     * @return Výsledok testu: true, ak je karta otočená lícom hore.
     */
    boolean isTurnedFaceUp();

    /**
     * Testuje, či má karta podobnu farbu ako zadaná karta.
     * Podobnou farbou se myslí čierna alebo červená.
     * @param c - Karta na porovnanie.
     * @return Výsledok testu: true, ak sú farby kariet podobné.
     */
    boolean similarColorTo(Card c);

    /**
     * Otočí kartu lícom hore.
     * @return Informacia, či došlo k otočeniu karty (=true) alebo nie.
     */
    boolean turnFaceUp();


    /**
     * @return Hodnotu karty.
     */
    int value();

    /**
     * @return Farbu karty
     */
    Card.Color color();

    /**
     * Zistí farbu karty (čierná alebo červená).
     * @return Farbu karty (ColorGroup).
     */
    Card.ColorGroup getColorGroup();

    /**
     * Vytvorí reťazec, ktorý je cestou k obrázku karty.
     * @return Url obrázku karty.
     */
    String getImage();

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


}