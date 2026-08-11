package protocols;

//Core actor class representing agents in the system
class Actor {
 private String id;
 private Point position;
 private Goal goal;
 
 public Actor(String id, Point position, Goal goal) {
     this.id = id;
     this.position = position;
     this.goal = goal;
 }
 
 public Goal getGoal() {
     return goal;
 }
 
 public Point getPosition() {
     return position;
 }
}