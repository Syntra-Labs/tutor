import java.util.ArrayList;

public class Player {
	private String name;
	private int health;
	private ArrayList<String> memories;
	private ArrayList<String> inventory;
	
	public Player(String name) {
		this.name = name;
		this.health = 100;
		this.inventory = new ArrayList<>();
	}
	
	public String getName() {
		return name;
	}
	
	public int getHealth() {
		return health;
	}
	
	public void takeDamage(int damage) {
		health -= damage;
		if (health < 0) health = 0;
	}
	
	public void addItem(String item) {
		inventory.add(item);
	}
	
	public ArrayList<String> getInventory() {
		return inventory;
	}
}
