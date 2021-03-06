const {
	start,
	init,
	onShouldQuit,
	stop,
	windowNew,
	windowShow,
	windowOnClosing,
	windowClose,
	windowGetTitle,
	windowSetTitle
} = require('libui-napi');

init();
onShouldQuit(() => {
	stop();
});
const win = windowNew('Test Window', 800, 600, false);

windowOnClosing(win, () => {
	if (windowGetTitle(win) == 'Test Window') {
		return windowSetTitle(win, 'Riprova');
	}
	console.log('closing', windowGetTitle(win));
	windowClose(win);
	stop();
});

windowShow(win, 42)
start();
