define(function(require,exports,module){
	$.fn.AntLine = function(){
        var borTop = $('<div/>'),borBottom = $('<div/>'),borLeft = $('<div/>'),borRight = $('<div/>');
        var cssText_1 = {width:30000,left:-15000,borderTop:'3px dashed #cacccf',position:'absolute',height:1},
        	cssText_2 = {position:'absolute',height:30000,top:-15000,borderLeft:'3px dashed #cacccf'}
    	borTop.css({top:0}).css(cssText_1);
    	borBottom.css({bottom:0}).css(cssText_1);
    	borLeft.css({left:0}).css(cssText_2);
    	borRight.css({right:0}).css(cssText_2);
    	$(this).css({position:'relative',zoom:1,overflow:'hidden'}).append(borTop,borBottom,borLeft,borRight);
        var left = borTop.position().left,
            top = borLeft.position().top;
        setInterval(function () {
            if (left < 0) {
                left += 2;
                borRight.css({top:left})
                borTop.css({left:left})
            }else left = -15000;
            if (top > -30000) {
                top -= 2;
                borBottom.css({left:top})
                borLeft.css({top:top})
            } else top = -15000;
        }, 60);
	};
})