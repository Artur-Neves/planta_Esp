

// faz qa requisição
async function getData(){
	// await ele espera pela requisição 
    const res = await fetch(`http://10.42.0.60/`);
    const data = await res.text();

    return data;
}
// mostra os dados da requisição
async function showDados(){
    const data = await getData();
    if (data!=null){ 
    var tempAmbiente = data.split('-')[0];
    var umidadeAmbiente = data.split('-')[1];
    var umidadeSolo = data.split('-')[2];
    var luminosidade = data.split('-')[3];
    var tempSolo = data.split('-')[4];
    var sensorFluxo = data.split('-')[5];
    var bordatmpambiente = document.getElementById("borda_tmpa");
var bordatmpsolo = document.getElementById("borda_tmps");
var bordaumidadeambiente = document.getElementById("borda_uma");
var bordaumidadesolo = document.getElementById("borda_ums");
var bordaluminosidade = document.getElementById("borda_lum");
var bordafluxo = document.getElementById("borda_fa");
var tempAmbienteElement = document.getElementById("tempAmbiente");
var tempSoloElement = document.getElementById("tempSolo");
var umidadeAmbienteElement = document.getElementById("umidadeAmbiente");
var umidadeSoloElement = document.getElementById("umidadeSolo");
var luminosidadeElement = document.getElementById("luminosidade");
var fluxoAguaElement = document.getElementById("fluxoAgua");
    tempAmbienteElement.textContent = tempAmbiente+ " °C";
    tempSoloElement.textContent = tempSolo + " °C";
    umidadeAmbienteElement.textContent = umidadeAmbiente + " %";
    umidadeSoloElement.textContent = umidadeSolo + " %";
    luminosidadeElement.textContent = luminosidade + " %";
    fluxoAguaElement.textContent = sensorFluxo + " L";

function setStyle(element, value, borda) {
    var valor=0;
    switch (element) {
        case tempAmbienteElement:
            valor=22;
            break;
            case tempSoloElement:
            valor=40;
            break;
            case umidadeSoloElement:
            valor = 75;
            break;
    
    
        default:
            valor=0;
            break;
    }
    if (value > valor) {
        borda.style.borderColor = 'red'; // Define a borda como vermelha se o valor for igual a 0
        element.style.color = "red"; // Define a cor do texto como vermelha se o valor for igual a 0
    } 
    else {
        borda.style.borderColor = "";
        element.style.color="";
    }
}
    setStyle(tempAmbienteElement, tempAmbiente, bordatmpambiente);
    setStyle(tempSoloElement, tempSolo, bordatmpsolo);
    setStyle(umidadeSoloElement, umidadeSolo, bordaumidadesolo);

}}
// intervalo é tipo um delay
let interval = setInterval(showDados, 100);
