package bestseller;
/**
 * Date: February 15, 2025 
 * Authors: Yassine Amraoui (041044570) & Sheilla Shojaie (41171088) 
 * Section Number: 303 
 * Lab Professor: Paulo Sousa 
 * Description: CST8132 25W: Object-Oriented Programming | Assignment 01: BookList
 * 
 * A custom Exception class for handling errors related to Book objects.
 * 
 */

public class BookException extends Exception {

	private static final long serialVersionUID = 1L;

	/*
	 * Constructs a new BookException with the specified message.
	 */
	public BookException(String message) {
		super("BookException: " + message);
	}
}
