class GUI implements java.awt.event.ActionListener,javax.swing.event.ChangeListener{
  
  int width = 480;
  BrightEyesEditor editor;
  
  JFrame window;
  JButton newDoc,saveDoc,prevFrame,nextFrame,addFrame,removeFrame,duplicateFrame,clearFrame,copyFrame,pasteFrame;
  JToggleButton mode,loop,help;
  JSlider ledBrightness,ledWidth,ledHeight,timeline;
  Box all;
  
  GUI(BrightEyesEditor editor){
    this.editor = editor;
    initGUI();
  }
  void initGUI(){
    window = new JFrame("Controls");
    window.setPreferredSize(new Dimension(width,editor.height+100));
    editor.frame.setVisible(false);
    editor.frame.setSize(1,1);
    
    window.setResizable(false);
    window.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
    
    all = new Box(BoxLayout.Y_AXIS);
    Box doc = new Box(BoxLayout.X_AXIS);
    Box led = new Box(BoxLayout.Y_AXIS);
    Box frame = new Box(BoxLayout.Y_AXIS);
    Box about = new Box(BoxLayout.X_AXIS);
    //document controls
    newDoc = (JButton)addButton("new(n)",doc,false);
    saveDoc = (JButton)addButton("save(s)",doc,false);
    //led cotrols
    Box modeBox = new Box(BoxLayout.X_AXIS);
    modeBox.add(new JLabel("mode: "));
    mode = (JToggleButton) addButton("fill",modeBox,true);
    mode.setSelected(true);
    
    Box brightnessBox = new Box(BoxLayout.X_AXIS);
    brightnessBox.add(new JLabel(" brightness(b/B): "));
    ledBrightness = new JSlider(JSlider.HORIZONTAL,0,255,192);
    brightnessBox.add(ledBrightness);
    
    Box widthBox = new Box(BoxLayout.X_AXIS);
    widthBox.add(new JLabel(" width(A/D): "));
    ledWidth = new JSlider(JSlider.HORIZONTAL,0,15,1);
    widthBox.add(ledWidth);
    
    Box heightBox = new Box(BoxLayout.X_AXIS);
    heightBox.add(new JLabel(" height(W/S): "));
    ledHeight = new JSlider(JSlider.HORIZONTAL,0,7,1);
    heightBox.add(ledHeight);
    
    led.add(modeBox);
    led.add(brightnessBox);
    led.add(widthBox);
    led.add(heightBox);
    //frame controls
    loop = (JToggleButton)addButton("loop( )",frame,true);
    Box timelineBox = new Box(BoxLayout.X_AXIS);
    prevFrame = (JButton)addButton("<-",timelineBox,false);
    timeline = new JSlider(JSlider.HORIZONTAL,0,0,0);
    timelineBox.add(timeline);
    nextFrame = (JButton)addButton("->",timelineBox,false);
    
    Box framesBox = new Box(BoxLayout.X_AXIS);
    Box addRemove = new Box(BoxLayout.Y_AXIS);
    Box duplicateClear = new Box(BoxLayout.Y_AXIS);
    Box copyPaste = new Box(BoxLayout.Y_AXIS);
    framesBox.add(addRemove);
    framesBox.add(duplicateClear);
    framesBox.add(copyPaste);
    
    addFrame = (JButton)addButton("add frame(=)",addRemove,false);
    removeFrame = (JButton)addButton("remove frame(-)",addRemove,false);
    
    duplicateFrame = (JButton)addButton("duplicate frame(+)",duplicateClear,false);
    clearFrame = (JButton)addButton("clear frame(c)",duplicateClear,false);
    
    copyFrame = (JButton)addButton("copy frame(ALT+c)",copyPaste,false);
    pasteFrame = (JButton)addButton("paste frame(ALT+v)",copyPaste,false);
    
    frame.add(timelineBox);
    frame.add(framesBox);
    
    JLabel copy = new JLabel(" TWSU Bright Eyes Editor");
    copy.setEnabled(false);
    about.add(copy);
    about.add(Box.createHorizontalGlue());
    help = (JToggleButton)addButton("help(?)",about,true);
    help.setSelected(true);
    
    all.setAlignmentX(Component.LEFT_ALIGNMENT);
    all.add(doc);
    all.add(new JSeparator(JSeparator.HORIZONTAL));
    all.add(led);
    all.add(new JSeparator(JSeparator.HORIZONTAL));
    all.add(frame);
    all.add(new JSeparator(JSeparator.HORIZONTAL));
    all.add(about);
    
    window.add(all);
    window.pack();
    //add listeners
    newDoc.addActionListener(this);
    saveDoc.addActionListener(this);
    mode.addActionListener(this);
    ledBrightness.addChangeListener(this);
    ledWidth.addChangeListener(this);
    ledHeight.addChangeListener(this);
    loop.addActionListener(this);
    prevFrame.addActionListener(this);
    timeline.addChangeListener(this);
    nextFrame.addActionListener(this);
    addFrame.addActionListener(this);
    removeFrame.addActionListener(this);
    duplicateFrame.addActionListener(this);
    clearFrame.addActionListener(this);
    copyFrame.addActionListener(this);
    pasteFrame.addActionListener(this);
    help.addActionListener(this);
  }
  void toggle(){
    if(window.isShowing()) window.hide();
    else                   window.show();
  }
  void update(){
    ledBrightness.setValue(editor.brightness);
    loop.setSelected(editor.autoUpdate);
    help.setSelected(editor.showHelp);
    updateTimelineSlider(); 
  }
  void updateTimelineSlider(){
    if(timeline.getMaximum() != editor.totalFrames) {
      timeline.removeChangeListener(this);
      timeline.setMaximum(editor.totalFrames-1);
      timeline.addChangeListener(this);
    }
    if(timeline.getValue() != editor.currentFrame) timeline.setValue(editor.currentFrame);
  }
  AbstractButton addButton(String text, Container container,boolean isToggle) {
      AbstractButton button = isToggle ? new JToggleButton(text) : new JButton(text);
      container.add(button);
      return button;
  }
  //handler implementations
  public void actionPerformed(java.awt.event.ActionEvent e) {
    Object source = e.getSource(); 
    if(source == newDoc)  editor.clear();
    if(source == saveDoc) editor.saveAnimation();
    if(source == mode) {
      boolean selected = mode.isSelected();
      mode.setLabel(selected ? "fill" : "erase");
      editor.erase = !selected;
    }
    if(source == loop) editor.autoUpdate = loop.isSelected();
    if(source == prevFrame && editor.currentFrame > 0) {
      editor.currentFrame--;
      timeline.setValue(currentFrame);
    }
    if(source == nextFrame && editor.currentFrame < editor.totalFrames-1) {
      editor.currentFrame++;
      timeline.setValue(currentFrame);
    }
    if(source == addFrame) {
      editor.addFrame();
      updateTimelineSlider();
    }
    if(source == removeFrame && editor.totalFrames > 1) {
      editor.removeFrame();
      updateTimelineSlider();
    }
    if(source == duplicateFrame) {
      editor.cloneFrame();
      updateTimelineSlider();
    }
    if(source == clearFrame) editor.clearFrame(); 
    if(source == copyFrame) editor.copyFrame();
    if(source == pasteFrame) editor.pasteFrame();
    if(source == help) showHelp = help.isSelected();
  } 
  public void stateChanged(javax.swing.event.ChangeEvent e) {
    Object source = e.getSource();
    if(source == ledBrightness) editor.brightness = ledBrightness.getValue();
    if(source == ledWidth)  editor.px = ledWidth.getValue() * 10;
    if(source == ledHeight) editor.py = ledHeight.getValue() * 22;
    if(source == timeline) editor.currentFrame = timeline.getValue();
  }
  
}
