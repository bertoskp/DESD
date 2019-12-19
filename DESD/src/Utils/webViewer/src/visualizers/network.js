export function getNetworkConfig(j) {
  const jNet = j['network'];
  const states = [];
  let transitions = {};
  const links = {};

  jNet['components'].forEach(c => {
    states.push({ id: c['name'] });
    c['transitions'].forEach(t => {
      transitions[t.name] = { id: t.name };
      if (t.hasOwnProperty('in') && t['in'] !== null) {
        const linkName = t['in'].link;
        if (!(linkName in links)) links[linkName] = {
          id: linkName,
          link: true
        };
        links[linkName].target = c['name'];
      }
      if (t.hasOwnProperty('out'))
        t['out'].forEach(l => {
          const linkName = l['link'];
          if (!(linkName in links)) links[linkName] = {
            id: linkName,
            link: true
          };
          links[linkName].source = c['name'];
        })
    });
    c['states'].forEach(s => {
      const id = c['name'] + '-' + s['name'];
      states.push({
        id,
        parent: c['name'],
        starting: s.hasOwnProperty('starting')
      });
      if (s.hasOwnProperty('in')) s['in'].forEach(tIn => transitions[tIn].target = id);
      if (s.hasOwnProperty('out')) s['out'].forEach(tOut => transitions[tOut].source = id)
    })
  });

  const elements = states.map(s => {
    return { data: s }
  });
  Object.keys(transitions).forEach(k => elements.push({
    data: transitions[k]
  }));
  Object.keys(links).forEach(k => elements.push({
    data: links[k]
  }));

  return {
    elements: elements,
    style: [ // the stylesheet for the graph
      {
        selector: 'node',
        style: {
          'background-color': el => {
            if (!el._private.data.hasOwnProperty('parent')) return '#EEEEEE';
            if (el._private.data.starting) return '#4444CC';
            if (el._private.data.final) return '#44CC44';
            return '#666666'
          },
          'label': 'data(id)',
          'text-valign': 'center'
        }
      },
      {
        selector: 'edge',
        style: {
          'width': 2,
          'line-color': '#ccc',
          'target-arrow-color': '#ccc',
          'curve-style': 'bezier',
          'target-arrow-shape': el => el._private.data.link !== undefined ? 'none' : 'triangle',
          'target-endpoint': 'outside-to-node',
          'label': 'data(id)'
        }
      }
    ],
    layout: {
      name: 'cose-bilkent',
      nodeDimensionsIncludeLabels: false,
      refresh: 30,
      fit: true,
      padding: 10,
      randomize: true,
      nodeRepulsion: 4500,
      idealEdgeLength: 80,
      edgeElasticity: 0.45,
      nestingFactor: 0.1,
      gravity: 0.25,
      numIter: 2500,
      tile: true,
      animate: 'during',
      tilingPaddingVertical: 10,
      tilingPaddingHorizontal: 10,
      gravityRangeCompound: 1.5,
      gravityCompound: 0.7,
      gravityRange: 3.8,
      initialEnergyOnIncremental: 0.5
    }
  };
}