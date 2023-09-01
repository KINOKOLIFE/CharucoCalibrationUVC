function aruoc4x4 (id){
    var obj = [[181,50],[15,154],[51,45],[153,70],[84,158],[121,205],[158,46],[196,242],[254,218],[207,86],[249,145],[17,167],[14,183],[42,15],[36,177],[38,62],[70,101],[102,0],[108,94],[118,175],[134,139],[176,43],[204,213],[221,130],[254,71],[148,113],[172,228],[165,84],[33,35],[52,111],[68,21],[87,178],[158,207],[240,203],[8,174],[9,41],[24,117],[4,255],[13,246],[28,90],[23,24],[42,40],[50,140],[56,178],[36,232],[46,235],[45,63],[75,100],[80,46],[80,19],[81,148],[85,104],[93,65],[95,151],[104,1],[104,103],[97,36],[97,233],[107,18],[111,229],[103,223],[126,27],[128,160],[131,68],[139,162],[147,122],[132,108],[133,42],[133,156],[156,137],[159,161],[187,124],[188,4],[182,91],[191,200],[183,171],[202,31],[201,98],[217,88],[211,213],[204,152],[199,160],[197,55],[233,93],[249,37],[251,187],[238,42],[247,77],[53,117],[138,173],[118,23],[10,207],[6,75],[45,193],[73,216],[67,244],[79,54],[79,211],[105,228],[112,199],[122,110],[180,234],[237,79],[252,231],[254,166],[0,37],[0,67],[10,136],[10,134],[2,111],[0,28],[0,151],[8,55],[10,49],[9,198],[11,1],[9,251],[11,88],[16,130],[24,45],[16,120],[16,115],[18,116],[18,177],[26,249],[19,6],[12,14],[12,241],[4,51],[12,159],[14,242],[14,253],[7,76],[15,164],[7,47],[5,181],[15,145],[7,219],[30,228],[20,57],[29,128],[21,200],[31,139],[21,186],[29,177],[32,128],[40,233],[34,162],[40,83],[42,240],[34,247],[41,64],[33,70],[41,185],[43,156],[43,178],[56,202],[56,46],[48,7],[56,231],[58,73],[58,101],[50,93],[59,136],[57,29],[59,211],[38,71],[39,128],[47,170],[45,20],[37,222],[37,83],[47,119],[52,72],[60,168],[60,65],[52,13],[52,251],[54,154],[61,224],[53,106],[61,9]];
    var u1 = obj[id][0];// [marker id][0]
    var u2 = obj[id][1];//[marker id][1]
    var u3 = u1 << 8;//
    u3 += u2;
    var data = [   0,0,0,0,0,0,
        0,0,0,0,0,0,
        0,0,0,0,0,0,
        0,0,0,0,0,0,
        0,0,0,0,0,0,
        0,0,0,0,0,0];
    var maskbit = 32768; //1000 0000 0000 0000
    var cot = 0;
    for( var row = 0; row < 4; row ++){
        for( var col = 0; col < 4; col ++){
            var st = u3&maskbit;
            if(st == 0){
                data[col+1 + (row + 1) * 6] = 0;
            }else{
                data[col+1 + (row + 1) * 6] = 1;
    
            }
            u3 = u3 << 1;
        }
    }
    return data;
}

function drawAruco4x4(id, size, cx, cy, pathItem){
    var raw = 4.0;
    var mm = 2.834645
    var step = size / (raw + 2);
    var data = aruoc4x4 (id);
    for( var y = 0; y < 6; y++){
        for( var x = 0; x < 6; x++){
            var n = x + y * 6;
            var rectx = step * x * mm + cx * mm ;
            var recty = step * y * mm + cy * mm ;
            var rect = pathItem.rectangle(-recty, rectx, step * mm, step * mm);
            rect.fillColor = new RGBColor(0,0,0);
            rect.stroked = false;
            if(data[n] == 0){
                rect.filled = true;
            }else{
                rect.filled = false;
            }
        }
    }
}

var doc = app.activeDocument;
var layerObj = doc.activeLayer;
var mm = 2.834645;
var offset_xmm = 10.0;
var offset_ymm = 10.0;
var grid_row = 10;
var grid_col = 7;
var grid_size = 26;
//-----
var aruco_id = 0;
var aruco_separation = 2;
var aruco_length = 22;

//drawAruco4x4(1, 28, 100 , 100, layerObj.pathItems);

for( var y = 0; y < grid_row; y++){
    for( var x = 0; x < grid_col; x++){
        var rectx = grid_size * x  + offset_xmm ;
        var recty = grid_size * y  + offset_ymm;
        var rect = layerObj.pathItems.rectangle(  -recty *  mm,  rectx   * mm , grid_size * mm , grid_size * mm);
        var sireal = x + y * 7;
        //var id = Math.trunc(sireal / 2.0);
        rect.fillColor = new RGBColor(0,0,0);
        rect.stroked = false;
        if (sireal % 2 == 0){
            rect.filled = true;
        }else{
            rect.filled = false;
            var aruco_rectx = rectx + aruco_separation;
            var aruco_recty = recty + aruco_separation;
            drawAruco4x4(aruco_id, aruco_length, aruco_rectx , aruco_recty, layerObj.pathItems);
            aruco_id ++;
        }
       
    }
}

