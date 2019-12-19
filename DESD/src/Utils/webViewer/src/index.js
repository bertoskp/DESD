import cytoscape from 'cytoscape';
import klay from 'cytoscape-klay';
import coseBilkent from 'cytoscape-cose-bilkent';
import { getNetworkConfig } from './visualizers/network'
import { getSimpleNetworkConfig } from './visualizers/simpleNetwork'
import { getBSConfig } from './visualizers/bs'
import { getDicConfig } from './visualizers/dic'
import { cyToPng } from './utils/cyToPng'

cytoscape.use(klay);
cytoscape.use(coseBilkent);
let cy;

const handleFileSelect = evt => {
  const files = evt.target.files;
  displayGraph(files[0]);
}

const displayGraph = file => {
  const reader = new FileReader();
  reader.addEventListener('loadend', () => {
    const j = JSON.parse(reader.result);
    const meta = j['meta'];

    const handlers = {
      'bs': getBSConfig,
      'network': getNetworkConfig,
      'dictionary': getDicConfig,
      'simplenetwork': getSimpleNetworkConfig
    }
    const handler = handlers[meta.type];

    if (handler == undefined) {
      alert('Unable to open this type of file.');
      return;
    }

    const cyConfig = handler(j);

    cy = cytoscape({
      container: document.getElementById('graph'),
      ...cyConfig,
    });
  });
  reader.readAsText(file);
}

document.getElementById('files').addEventListener('change', handleFileSelect, false);
document.getElementById('export').addEventListener('click', () => cyToPng(cy), false);