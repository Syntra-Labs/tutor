package simulation;

import java.io.IOException;
import java.util.Scanner;

public class ASCIIGame {
    private static final int MAP_WIDTH = 20;
    private static final int MAP_HEIGHT = 10;
    private static char[][] map;
    private static int playerX;
    private static int playerY;
    private static boolean isGameRunning = true;

    public static void main(String[] args) {
        initializeGame();
        runGame();
    }

    private static void runGame() {
        Scanner scanner = new Scanner(System.in);
        System.out.println("Use WASD to move, Q to quit");
        
        while (isGameRunning) {
            drawMap();
            
            // Get input without requiring Enter
            try {
                // Check if input is available
                if (System.in.available() > 0) {
                    char input = (char) System.in.read();
                    processInput(input);
                    // Clear any remaining characters (like newlines)
                    while (System.in.available() > 0) {
                        System.in.read();
                    }
                }
                Thread.sleep(50); // Add a small delay to prevent high CPU usage
            } catch (IOException | InterruptedException e) {
                e.printStackTrace();
            }
        }
        
        scanner.close();
        System.out.println("Game Over!");
    }

    private static void initializeGame() {
        map = new char[MAP_HEIGHT][MAP_WIDTH];
        
        // Initialize empty map
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                map[y][x] = '.';
            }
        }
        
        // Set player initial position
        playerX = MAP_WIDTH / 2;
        playerY = MAP_HEIGHT / 2;
        map[playerY][playerX] = 'P';
        
        // Add some walls
        for (int i = 0; i < 10; i++) {
            int wallX = (int) (Math.random() * MAP_WIDTH);
            int wallY = (int) (Math.random() * MAP_HEIGHT);
            if (wallX != playerX || wallY != playerY) {
                map[wallY][wallX] = '#';
            }
        }
    }

    private static void drawMap() {
        // Clear console using Windows command
        try {
            new ProcessBuilder("cmd", "/c", "cls").inheritIO().start().waitFor();
        } catch (Exception e) {
            // If clearing fails, print newlines as fallback
            for (int i = 0; i < 50; i++) System.out.println();
        }
        
        // Draw border top
        System.out.print("+");
        for (int x = 0; x < MAP_WIDTH; x++) {
            System.out.print("-");
        }
        System.out.println("+");
        
        // Draw map
        for (int y = 0; y < MAP_HEIGHT; y++) {
            System.out.print("|");
            for (int x = 0; x < MAP_WIDTH; x++) {
                System.out.print(map[y][x]);
            }
            System.out.println("|");
        }
        
        // Draw border bottom
        System.out.print("+");
        for (int x = 0; x < MAP_WIDTH; x++) {
            System.out.print("-");
        }
        System.out.println("+");
        
        // Show controls reminder
        System.out.println("WASD to move, Q to quit");
    }

    private static void processInput(char input) {
        int newX = playerX;
        int newY = playerY;
        
        switch (Character.toLowerCase(input)) {
            case 'w': newY--; break;
            case 's': newY++; break;
            case 'a': newX--; break;
            case 'd': newX++; break;
            case 'q': isGameRunning = false; return;
            default: return;
        }
        
        // Check if new position is valid
        if (isValidMove(newX, newY)) {
            // Update player position
            map[playerY][playerX] = '.';
            playerX = newX;
            playerY = newY;
            map[playerY][playerX] = 'P';
        }
    }

    private static boolean isValidMove(int x, int y) {
        return x >= 0 && x < MAP_WIDTH && 
               y >= 0 && y < MAP_HEIGHT && 
               map[y][x] != '#';
    }
}