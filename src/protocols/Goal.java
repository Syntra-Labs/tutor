package protocols;

//Goal definition for actors
class Goal {
 private String description;
 private double targetValue;
 
 public Goal(String description, double targetValue) {
     this.description = description;
     this.targetValue = targetValue;
 }
 
 public double getTargetValue() {
     return targetValue;
 }
}