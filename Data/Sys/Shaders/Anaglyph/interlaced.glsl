// Interlaced stereoscopy

void main()
{
	int row = int(gl_FragCoord.y) - (GetWindowPos().y % 2);
	SetOutput(SampleLayer(row % 2));
}
