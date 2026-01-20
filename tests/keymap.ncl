let { tap, hold, td, combo, encoder,  MO, .. } = import "fak/keycode.ncl" in

# We alias commonly used stuff, so we don't end up repeating them over and over.
let kc = tap.reg.kc in
let me = tap.custom.media in
let XXXX = tap.none & hold.none in
let TTTT = tap.trans & hold.trans in

# Keymap definition
{
  virtual_keys = [
    encoder.ccw 0,
    encoder.cw 0,
  ],

  layers = [
    [ # Layer 0
      kc.F1, kc.F2, kc.F3,
      me.PREV, me.PLAY, me.NEXT,
      MO 1,
      # Combos start here in the same order we defined them above
      me.VOLU, me.VOLD,
    ],
    [ # Layer 1
      TTTT,    TTTT,    TTTT,
      TTTT,    TTTT,    TTTT,
      TTTT, 
      # Combos start here in the same order we defined them above
      kc.F4, kc.F5,
    ],
  ]
}
