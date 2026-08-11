package bestseller;
/**
 * Date: February 15, 2025 
 * Authors: Yassine Amraoui (041044570) & Sheilla Shojaie (41171088) 
 * Section Number: 303 
 * Lab Professor: Paulo Sousa 
 * Description: CST8132 25W: Object-Oriented Programming | Assignment 01: BookList
 * 
 * A simple Book class to represent a book with attributes of name, author,
 * original language, year of first publication, sales in millions, and genre.
 * It provides methods to get and set these attributes and to print the book
 * fields.
 * 
 */

public class Book {
	private int index;
	private String name;
	private String author;
	private String originalLanguage;
	private int firstPublished;
	private float millionSales;
	private String genre;

	/*
	 * Constructs a new Book object with the given attributes.
	 */
	public Book(int index, String name, String author, String originalLanguage, int firstPublished, float millionSales,
			String genre) {
		this.index = index;
		this.name = name;
		this.author = author;
		this.originalLanguage = originalLanguage;
		this.firstPublished = firstPublished;
		this.millionSales = millionSales;
		this.genre = genre;
	}

	/*
	 * Returns the index of the Book.
	 */
	public int getIndex() {
		return index;
	}

	/*
	 * Returns the name of the Book.
	 */
	public String getName() {
		return name;
	}

	/*
	 * Returns the author of the Book.
	 */
	public String getAuthor() {
		return author;
	}

	/*
	 * Returns the original language of the Book.
	 */
	public String getOriginalLanguage() {
		return originalLanguage;
	}

	/*
	 * Returns the year of first publication of the Book.
	 */
	public int getFirstPublished() {
		return firstPublished;
	}

	/*
	 * Returns the sale in millions of the Book.
	 */
	public float getMillionSales() {
		return millionSales;
	}

	/*
	 * Returns the genre of the Book.
	 */
	public String getGenre() {
		return genre;
	}

	/*
	 * Sets the index of the Book.
	 */
	public void setIndex(int index) {
		this.index = index;
	}

	/*
	 * Sets the name of the Book.
	 */
	public void setName(String name) {
		this.name = name;
	}

	/*
	 * Sets the author of the Book.
	 */
	public void setAuthor(String author) {
		this.author = author;
	}

	/*
	 * Sets the original language of the Book.
	 */
	public void setOriginalLanguage(String originalLanguage) {
		this.originalLanguage = originalLanguage;
	}

	/*
	 * Sets the year of first publication of the Book.
	 */
	public void setFirstPublished(int firstPublished) {
		this.firstPublished = firstPublished;
	}

	/*
	 * Sets the sales in millions of the Book.
	 */
	public void setMillionSales(float millionSales) {
		this.millionSales = millionSales;
	}

	/*
	 * Sets the genre of the Book.
	 */
	public void setGenre(String genre) {
		this.genre = genre;
	}

	/*
	 * Returns a String representation of the fields of the Book.
	 */
	@Override
	public String toString() {
		String formattedMillionSales = formatMillionSales(millionSales);
		return String.format("Index: %d, Name: %s, Author: %s, Language: %s, Published: %d, Sales: %s, Genre: %s",
				index, name, author, originalLanguage, firstPublished, formattedMillionSales, genre);
	}

	/*
	 * Formats millionSales as a String keeping the fractional part if there is one.
	 */
	public String formatMillionSales(float millionSales) {
		String formattedMillionSales = "";
		if (millionSales % 1 == 0) {
			formattedMillionSales = String.format("%.0f", millionSales);
		} else {
			formattedMillionSales = String.format("%.1f", millionSales);
		}
		return formattedMillionSales;
	}

}