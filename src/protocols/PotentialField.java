package protocols;

//Value object to represent potential field at a point
class PotentialField {
 Point position;
 double potential;
 
 PotentialField(Point position, double potential) {
     this.position = position;
     this.potential = potential;
 }
}