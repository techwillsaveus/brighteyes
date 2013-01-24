// storage class for each LED node
private class node {
  public int leftX;
  public int rightX;
  public int topY;
  public int bottomY;

  node(int leftX, int rightX, int topY, int bottomY) {
    this.leftX = leftX;
    this.rightX = rightX;
    this.topY = topY;
    this.bottomY = bottomY;
  }
};

void setupCoords(){
  // load mapping XML 
  XML inputXML = loadXML("bright-eyes-coords.xml");
  XML[] coordsXML = inputXML.getChildren("node");
  XML sizeXML = inputXML.getChild("outputSize");

  // extract map width and height
  mapWidth = sizeXML.getInt("width");
  mapHeight = sizeXML.getInt("height");

  // set app size to match map
  size(mapWidth, mapHeight);
  println(mapWidth + " x " + mapHeight); 
  frameRate(30);
  // check we have a correct map
  if (coordsXML.length != numLeds) {
    print("Input XML length is ");
    print(coordsXML.length);
    print(" should be ");
    println(numLeds); 
    exit();
  }else {
    // if we do, create a node array
    nodes = new node[coordsXML.length];

    // populate the node array from the XML
    for (int i = 0; i < coordsXML.length; i++) {
      int address = coordsXML[i].getInt("address");
      int leftX = coordsXML[i].getInt("leftX");
      int rightX = coordsXML[i].getInt("rightX");
      int topY = coordsXML[i].getInt("topY");
      int bottomY = coordsXML[i].getInt("bottomY");
      nodes[address-1] = new node(leftX, rightX, topY, bottomY);
    }
  }
}
