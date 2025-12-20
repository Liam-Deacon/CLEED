Chapter 1: Introduction
=======================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 2-5). Figure references
   are placeholders pending review.

1.1 History
-----------

When Davisson and Germer reported in 1927 that the elastic scattering of low-energy
 electrons from well ordered surfaces leads to diffraction spots similar to those observed
 in X-ray diffraction [1, 2, 3], this was the first experimental proof of the wave nature of
 electrons. A few years before, in 1923, de'Broglie had postulated that electrons have a
 wavelength of

.. math::

   \lambda_e = \frac{h}{m_e v} = \sqrt{\frac{150}{E_{\mathrm{kin}}[\mathrm{eV}]}}\,\mathrm{\AA}
   \tag{1.1}

and a corresponding wave vector of the length

.. math::

   k = \frac{2\pi}{\lambda_e}
   \tag{1.2}

where :math:`h` is Planck's constant, :math:`m_e` the electron mass, :math:`v` the velocity,
 and :math:`E_{\mathrm{kin}}` the kinetic energy of the electron. Already Davisson and
 Germer realised that the diffraction of low-energy electrons (LEED) in the energy range
 between 40 and 500 eV, where their wavelength ranges between 0.5 and 2 \AA, can be used
 in order to determine the structure of single crystal surfaces in analogy to X-ray
 diffraction. Due to their small inelastic mean free path (IMFP) of only a few \AA
 (typically less than 10 \AA) electrons in this energy range sample only the top-most
 atomic layers of a surface and are, therefore, better suited for the analysis of surface
 geometries than X-ray photons which have a much larger mean free path (typically a few
 \mu m). However, unlike for photon diffraction, multiple scattering plays an important
 role in the diffraction process of electrons at solid surfaces. Therefore, the analysis of
 LEED data with respect to the exact positions of atoms at the surface is somewhat more
 complicated and requires fully dynamical quantum mechanical scattering calculations.

The use of LEED for surface analysis became important when large single crystals became
 available for surface studies. It was first only used for a qualitative characterisation
 of surface ordering and the quantitative determination of the two-dimensional surface
 lattice parameters (e.g. superstructures). The information about the positions of the
 atoms in the surface is hidden in the energy-dependence of the diffraction spot
 intensities, the so-called LEED :math:`I-V`, or :math:`I(E)`, curves. In the late 1960's
 computer programs became available which could perform fully dynamical scattering
 calculations for simple surface geometries. The comparison of such theoretical
 :math:`I-V` curves for a set of model geometries with experimental data allows the
 determination of the atomic positions within the surface by trial and error. With the
 immense growth of available computer power and speed since then, LEED :math:`I-V`
 structure determination could be applied to a large number of more and more complex
 surface geometries which had made LEED the standard technique of modern surface
 crystallography.

For further details about the history, experimental setup, and theoretical approaches of
 LEED please refer to the books by Pendry [4], Van Hove and Tong [5], Van Hove, Weinberg
 and Chan [6], and Clarke [7].

1.2 Instrumentation
-------------------

The standard modern LEED optics is of the "rear view" type which is schematically shown
 in Figure TODO (PDF p9). The incident electron beam, accelerated by the potential :math:`V_0`,
 is emitted from the electron gun behind the hemispherical fluorescent screen made of
 glass and hits the sample through a hole in the screen. The surface is in the centre of
 the hemisphere so that all back-diffracted electrons travel towards the LEED screen on
 radial trajectories. Before the electrons hit the screen they have to pass a retarding
 field energy analyser. It typically consists of four (or three) hemispherical grids
 concentric with the screen, each containing a central hole through which the electron
 gun is inserted. The first grid (nearest to the sample) is connected to earth ground, as
 is the sample, to provide an essentially field-free region between the sample and the
 first grid. This minimises an undesirable electrostatic deflection of diffracted
 electrons. A suitable negative potential (:math:`V_0 - V_{\mathrm{sup}}`) is applied to the
 second and third (only second) grids, the so-called suppressor grids, to allow a narrow
 energy range :math:`V_{\mathrm{sup}}` of elastically scattered electrons to be transmitted
 to the fluorescent screen. The fourth (third) grid is usually grounded in order to reduce
 field penetration of the suppressor grids by the screen voltage which is of the order of
 a few kV in order to make the diffraction spots visible. Since the fluorescent screen is
 transparent, the spots can be observed through a view-port behind the screen without
 being shadowed by the sample holder. Only the electron gun assembly (diameter < 15 mm)
 obstructs the view slightly. Older designs have opaque screens which allow only the
 observation from behind the sample. A typical diameter of the complete optics is around
 140 mm so that it fits into a 150 mm (ID) flange. To date, the usual way of recording the
 LEED pattern is a light-sensitive video camera with a suitable image processing system.
 In older systems movable Faraday cups (FC) were used which are detecting the electron
 current directly. Because of the long data acquisition times and the problems of
 transferring motion into UHV, these systems are mostly out of use nowadays.

Only one widely used type of LEED optics uses a Faraday cup arrangement as electron
 detector which is specially designed for an accurate spot profile analysis (SPA-LEED,
 cf. Figure TODO (PDF p10)) [8]. In this design the FC is at a fixed position and the grids are
 replaced by round apertures defining the lateral resolution of the system. Instead of
 the position of the electron detector, the angle of the incoming electron beam is varied
 through an electrostatic octupole lens, thereby deflecting the desired part of the LEED
 pattern towards the detector without the need of moving parts. This type of LEED optics
 is specially designed to have a large resolution in k-space. The better the k resolution
 is the larger are the typical distances on the surface for which effects can be observed
 in the LEED pattern (spot position, profile). The largest resolvable length on the surface
 is called the transfer width and characterises the LEED instrument [9]. Typical values
 are around 150 \AA for conventional rear view LEED systems and up to 1000 \AA for
 SPA-LEED systems.

The standard experimental setup for collecting LEED :math:`I-V` curves uses a video
 camera recording images of the fluorescent screen for each energy (Video LEED) [28].
 When a conventional video camera is used the rate at which images are collected is fixed
 at 50 to 60 Hz. In this case several images (typically between 8 and 256) have to be
 averaged in order to obtain a satisfying signal-to-noise ratio, especially for weak
 superstructure spots. Newer systems often use slow-scan CCD cameras which can perform
 the averaging directly on the cooled CCD chip (exposure times up to several seconds) and
 therefore avoid multiple readout noise [29]. The images are either stored on a computer
 hard disk and analysed in a second round or the spot intensities are extracted online by
 a special software. Usually, the intensity is averaged within a given area at the spot
 position and the averaged local background outside this area is subtracted from this
 intensity. Commercially available data acquisition software also performs the control of
 the electron energy and records the electron beam current which is needed in order to
 normalise the spot intensities.

Older experimental setups were using Faraday cups with small apertures mounted on
 goniometers which could be moved around the sample in order to collect the back-scattered
 electron current directly or spot photometers which were directed at one diffraction spot
 on the fluorescent screen which was then followed by hand while the energy was varied.
The data collection is mostly done at normal incidence of the primary electron beam. In
 this case, usually several equivalent LEED spots exist due to the surface symmetry. By
 taking care that the :math:`I-V` curves of equivalent spots are identical, normal incidence
 conditions can be adjusted within a few tenth of a degree.

.. figure:: /_static/manual/figures/figure-placeholder-p9.png
   :alt: Placeholder for LEED optics schematic (PDF p9).
   :align: center

   Figure TODO (PDF p9): Rear-view LEED optics schematic.

.. figure:: /_static/manual/figures/figure-placeholder-p10.png
   :alt: Placeholder for SPA-LEED optics schematic (PDF p10).
   :align: center

   Figure TODO (PDF p10): SPA-LEED Faraday cup arrangement.
